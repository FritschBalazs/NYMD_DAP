/*
 * debugger.c
 *
 *	Main application code for the CMSIS-DAP probe.
 *
 *  Created on: Jan 18, 2024
 *      Author: FBalazs
 */

#include "debugger.h"
#include "main.h"
#include "DAP_config.h"
#include "usbd_customhid.h"
#include "DAP.h"
#include "usbd_cdc_if.h"
#include "usart.h"

#define USB_HID_BUSY_USER_TIMEOUT (2U)
#define SWO_CAPTURE_BUFF_SIZE (1*4*8)  //4 word FIFO with byte packing

extern USBD_HandleTypeDef hUsbDeviceHS;
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;

/* I've copied and modified the main functions from LPC-Link2 V1 example from CMSIS-DAP.
 * The following variables are from there. They implement two circular buffers */
static volatile uint16_t USB_RequestIndexI;     // Request  Index In
static volatile uint16_t USB_RequestIndexO;     // Request  Index Out
static volatile uint16_t USB_RequestCountI;     // Request  Count In
static volatile uint16_t USB_RequestCountO;     // Request  Count Out

static volatile uint16_t USB_ResponseIndexI;    // Response Index In
static volatile uint16_t USB_ResponseIndexO;    // Response Index Out
static volatile uint16_t USB_ResponseCountI;    // Response Count In
static volatile uint16_t USB_ResponseCountO;    // Response Count Out
static volatile uint8_t  USB_ResponseIdle;      // Response Idle  Flag

static uint8_t  USB_Request [DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Request  Buffer (circular)
static uint8_t  USB_Response[DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Response Buffer (circular)

uint8_t SWO_buff_0[SWO_CAPTURE_BUFF_SIZE];
uint8_t SWO_buff_1[SWO_CAPTURE_BUFF_SIZE];
uint8_t* pActive_SWO_buff;					//indicates buffer that is currently receiving data
uint8_t SWO_buff_empty;						//flag to indicate that the unused buffer has been emptied
uint8_t SWO_buff_overrun_ERR;				//error flag
uint8_t CDC_Tx_ERR;						//CDC_Transimt_HS failed

bool REQUEST_FLAG = 0;
bool BUFFER_FULL_FLAG = 0;


/* Local function prototypes  */
void DAP_USB_Initialize (void);


/* debug variables */
volatile uint16_t dbg_cntr = 0;

// Called during USBD_Initialize to initialize the USB HID class instance.
void DAP_USB_Initialize (void) {
  // Initialize variables
  USB_RequestIndexI  = 0U;
  USB_RequestIndexO  = 0U;
  USB_RequestCountI  = 0U;
  USB_RequestCountO  = 0U;
  USB_ResponseIndexI = 0U;
  USB_ResponseIndexO = 0U;
  USB_ResponseCountI = 0U;
  USB_ResponseCountO = 0U;
  USB_ResponseIdle   = 1U;
}


// \brief Prepare HID Report data to send.
// \param[in]   rtype   report type:
//                - HID_REPORT_INPUT           = input report requested
//                - HID_REPORT_FEATURE         = feature report requested
// \param[in]   req     request type:
//                - USBD_HID_REQ_EP_CTRL       = control endpoint request
//                - USBD_HID_REQ_PERIOD_UPDATE = idle period expiration request
//                - USBD_HID_REQ_EP_INT        = previously sent report on interrupt endpoint request
// \param[in]   rid     report ID (0 if only one report exists).
// \param[out]  buf     buffer containing report data to send.
// \return              number of report data bytes prepared to send or invalid report requested.
//              - value >= 0: number of report data bytes prepared to send
//              - value = -1: invalid report requested
int32_t HID0_GetReport (uint8_t rtype, uint8_t req, uint8_t rid, uint8_t *buf) {
  (void)rid;

  switch (rtype) {
    case HID_REPORT_INPUT:
      switch (req) {
        case USBD_HID_REQ_EP_CTRL:        // Explicit USB Host request via Control OUT Endpoint
        case USBD_HID_REQ_PERIOD_UPDATE:  // Periodic USB Host request via Interrupt OUT Endpoint
          break;
        case USBD_HID_REQ_EP_INT:         // Called after USBD_HID_GetReportTrigger to signal data obtained.
          if (USB_ResponseCountI != USB_ResponseCountO) {
            // Load data from response buffer to be sent back
            memcpy(buf, USB_Response[USB_ResponseIndexO], DAP_PACKET_SIZE);
            USB_ResponseIndexO++;
            if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
              USB_ResponseIndexO = 0U;
            }
            USB_ResponseCountO++;
            return ((int32_t)DAP_PACKET_SIZE);
          } else {
            USB_ResponseIdle = 1U;
          }
          break;
      }
      break;
    case HID_REPORT_FEATURE:
      break;
  }
  return (0);
}


// \brief Process received HID Report data.
// \param[in]   rtype   report type:
//                - HID_REPORT_OUTPUT    = output report received
//                - HID_REPORT_FEATURE   = feature report received
// \param[in]   req     request type:
//                - USBD_HID_REQ_EP_CTRL = report received on control endpoint
//                - USBD_HID_REQ_EP_INT  = report received on interrupt endpoint
// \param[in]   rid     report ID (0 if only one report exists).
// \param[in]   buf     buffer that receives report data.
// \param[in]   len     length of received report data.
// \return      true    received report data processed.
// \return      false   received report data not processed or request not supported.
bool HID0_SetReport (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len) {
  (void)req;
  (void)rid;

  switch (rtype) {
    case HID_REPORT_OUTPUT:
      if (len == 0) {
        break;
      }
      if (buf[0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1U;
        break;
      }
      if ((uint16_t)(USB_RequestCountI - USB_RequestCountO) == DAP_PACKET_COUNT) {
        BUFFER_FULL_FLAG = 1;
        break;  // Discard packet when buffer is full
      }
      // Store received data into request buffer
      memcpy(USB_Request[USB_RequestIndexI], buf, (uint32_t)len);
      USB_RequestIndexI++;
      if (USB_RequestIndexI == DAP_PACKET_COUNT) {
        USB_RequestIndexI = 0U;
      }
      USB_RequestCountI++;
      REQUEST_FLAG = 1;
      break;
    case HID_REPORT_FEATURE:
      break;
  }
  return true;
}

/* Function to call from the uart receive callback.
 * It works on a doubble buffer prinicple. UART DMA is
 *    configured with fifo and byte packing
 *    to minimize periph bus usage when data
 *    is coming in constantly.
 * But DMA reception is handeled with a timout param,
 *    os if there is an inactive period on the uart line,
 *    the callback is called, and a timeout is indicated.
 *
 */
void capture_SWO_init(void){
	pActive_SWO_buff = SWO_buff_0;
	SWO_buff_empty = 1;
	SWO_buff_overrun_ERR = 0;
	CDC_Tx_ERR  = 0;
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, pActive_SWO_buff, SWO_CAPTURE_BUFF_SIZE);

}

HAL_StatusTypeDef SWO_setspeed(){

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (huart->Instance == USART1){
		while(1);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){


	if (huart->Instance == USART1){
		uint8_t* rx_buffer = pActive_SWO_buff;

		if (!SWO_buff_empty){
			SWO_buff_overrun_ERR = 1;
		}

		/* change buffers */
		if (pActive_SWO_buff == SWO_buff_0){
			pActive_SWO_buff = SWO_buff_1;
		}else{
			pActive_SWO_buff = SWO_buff_0;
		}
		//set flag to
		SWO_buff_empty = 0;
		/* start next reception */
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, pActive_SWO_buff, SWO_CAPTURE_BUFF_SIZE);

		if (CDC_Transmit_HS(rx_buffer, Size) == USBD_OK){
			SWO_buff_empty = 1;
		}else{
			CDC_Tx_ERR = 1;
		}


		if (huart->RxEventType == HAL_UART_RXEVENT_TC){ //Transmittion complete
			return;
		}
		if (huart->RxEventType == HAL_UART_RXEVENT_HT){ //Transmition half complete
			return;
		}
		if (huart->RxEventType == HAL_UART_RXEVENT_IDLE){
			return;

		}
	}
}



/* Wrapper for USBD_HID_SendReport(). It checks device state first, and calls remote wakeup if needed
 *
 * based on HID_Standalone wakeup example for f746G disco */
uint8_t HID_Send_Report(USBD_HandleTypeDef *pdev,uint8_t *report, uint16_t len){

/* once remoteWakeUp is supported properly, I will clean this up //TODO remoteWakeup?
	if ((pdev->dev_remote_wakeup == 1) && (pdev->dev_state == USBD_STATE_SUSPENDED)){
		if ((&hpcd_USB_OTG_HS)->Init.low_power_enable)
		{
			// Reset SLEEPDEEP bit of Cortex System Control Register
			SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

	        //SystemClockConfig_STOP();
		}

		// Ungate PHY clock
		__HAL_PCD_UNGATE_PHYCLOCK((&hpcd_USB_OTG_HS));

		// Activate Remote wakeup
		HAL_PCD_ActivateRemoteWakeup((&hpcd_USB_OTG_HS));

		// Remote wakeup delay
		HAL_Delay(10);

		// Disable Remote wakeup
		HAL_PCD_DeActivateRemoteWakeup((&hpcd_USB_OTG_HS));

		// change state to configured
		pdev->dev_state = USBD_STATE_CONFIGURED;

		// Change remote_wakeup feature to 0
		pdev->dev_remote_wakeup=0;
		//remotewakeupon = 1;

		printf("Remote wakeup \r\n");
	}
*/

	if (USBD_CUSTOM_HID_SendReport(pdev, report, len, CUSTOMHID_InstID) == CUSTOM_HID_BUSY){
		uint32_t timestamp = HAL_GetTick();
		while(USBD_CUSTOM_HID_SendReport(pdev, report, len, CUSTOMHID_InstID) != USBD_OK && (timestamp + USB_HID_BUSY_USER_TIMEOUT >= HAL_GetTick())){
			//wait
		}
	}

	return 0;

}

// just for test
uint32_t cnt = 0;
uint8_t text[CDC_DATA_HS_MAX_PACKET_SIZE] = {"Hello \r\n"};
uint8_t enable_send = 1;

/* Modified DAP thread from the example */
void APP_Run(void){
	uint32_t n;

	if(REQUEST_FLAG || BUFFER_FULL_FLAG){

		/*clear flags */
		REQUEST_FLAG = 0;
		BUFFER_FULL_FLAG = 0;

		// Process pending requests
		while (USB_RequestCountI != USB_RequestCountO) {

		  // Handle Queue Commands
		  n = USB_RequestIndexO;
		  while (USB_Request[n][0] == ID_DAP_QueueCommands) {
			USB_Request[n][0] = ID_DAP_ExecuteCommands;
			n++;
			if (n == DAP_PACKET_COUNT) {
			  n = 0U;
			}
			if (n == USB_RequestIndexI) {
				while(!REQUEST_FLAG || !BUFFER_FULL_FLAG){
					if (BUFFER_FULL_FLAG) {
						/*invalid request from host, abort */
						BUFFER_FULL_FLAG = 0;
						break;
					}
					REQUEST_FLAG = 0;
				}
			}
		  }

		  // Execute DAP Command (process request and prepare response)
		  DAP_ExecuteCommand(USB_Request[USB_RequestIndexO], USB_Response[USB_ResponseIndexI]);

		  // Update Request Index and Count
		  USB_RequestIndexO++;
		  if (USB_RequestIndexO == DAP_PACKET_COUNT) {
			USB_RequestIndexO = 0U;
		  }
		  USB_RequestCountO++;

		  // Update Response Index and Count
		  USB_ResponseIndexI++;
		  if (USB_ResponseIndexI == DAP_PACKET_COUNT) {
			USB_ResponseIndexI = 0U;
		  }
		  USB_ResponseCountI++;

		  if (USB_ResponseIdle) {
			if (USB_ResponseCountI != USB_ResponseCountO) {
			  // Load data from response buffer to be sent back
			  n = USB_ResponseIndexO++;
			  if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
				USB_ResponseIndexO = 0U;
			  }
			  USB_ResponseCountO++;
			  USB_ResponseIdle = 0U;
			  /* send data */
			  HID_Send_Report(&hUsbDeviceHS, USB_Response[n], DAP_PACKET_SIZE);
			}
		  }
		}
	}

	/* if HS transmit failed in callback try again here */
	if (SWO_buff_overrun_ERR){
		//if (CDC_Transmit_HS("ERR: Buffer overrun occured \n", 30) == USBD_OK){ //TODO cleanup
		if(printf("SWO buffer overrun ")){
			SWO_buff_overrun_ERR = 0;
			SWO_buff_empty = 1;
		}
		else{
			//printf("SWO buffer overrun ");
		}

	}

	if(CDC_Tx_ERR){
		//if (CDC_Transmit_HS("CDC TX error occured \n", 23) == USBD_OK){
		if(printf("CDC TX error occured \n")){
			CDC_Tx_ERR = 0;
			SWO_buff_empty = 1;
		}
		else{
			//printf("CDC TX error occured \n");
		}
	}

	//CDC testcode
	/*cnt++;
	if (cnt>=10000000){
		cnt = 0;
		if (enable_send){
			CDC_Transmit_HS(&text, sizeof(text));
		}
	}*/


}



void APP_Setup(void){
	DAP_Setup();                          // DAP Setup



	LED_CONNECTED_OUT(1U);                // Turn on  Debugger Connected LED
	LED_RUNNING_OUT(1U);                  // Turn on  Target Running LED
	Delayms(500U);                        // Wait for 500ms
	LED_RUNNING_OUT(0U);                  // Turn off Target Running LED
	LED_CONNECTED_OUT(0U);                // Turn off Debugger Connected LED //TODO connect LED

	capture_SWO_init();					  //start capturing SWO Data

}
