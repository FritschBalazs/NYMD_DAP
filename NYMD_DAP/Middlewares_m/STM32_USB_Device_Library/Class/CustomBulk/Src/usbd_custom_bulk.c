/**
  ******************************************************************************
  * @file    usbd_template.c
  * @author  MCD Application Team
  * @brief   This file provides the HID core functions.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  * @verbatim
  *
  *          ===================================================================
  *                                TEMPLATE Class  Description
  *          ===================================================================
  *
  *
  *
  *
  *
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_bulk.h"
#include "usbd_ctlreq.h"
#include "debugger.h"
#include "DAP_config.h"
/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_TEMPLATE
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_TEMPLATE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_TEMPLATE_Private_Defines
  * @{
  */
#define EP_ADDR_7F_MASK (0x7Fu)

/**
  * @}
  */


/** @defgroup USBD_TEMPLATE_Private_Macros
  * @{
  */

/**
  * @}
  */




/** @defgroup USBD_TEMPLATE_Private_FunctionPrototypes
  * @{
  */


static uint8_t USBD_TEMPLATE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_TEMPLATE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_TEMPLATE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_TEMPLATE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_TEMPLATE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_TEMPLATE_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_TEMPLATE_EP0_TxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_TEMPLATE_SOF(USBD_HandleTypeDef *pdev);
static uint8_t USBD_TEMPLATE_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_TEMPLATE_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t *USBD_TEMPLATE_GetCfgDesc(uint16_t *length);
static uint8_t *USBD_TEMPLATE_GetDeviceQualifierDesc(uint16_t *length);
/**
  * @}
  */

/** @defgroup USBD_TEMPLATE_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_TEMPLATE_ClassDriver =
{
  USBD_TEMPLATE_Init,
  USBD_TEMPLATE_DeInit,
  USBD_TEMPLATE_Setup,
  USBD_TEMPLATE_EP0_TxReady,
  USBD_TEMPLATE_EP0_RxReady,
  USBD_TEMPLATE_DataIn,
  USBD_TEMPLATE_DataOut,
  USBD_TEMPLATE_SOF,
  USBD_TEMPLATE_IsoINIncomplete,
  USBD_TEMPLATE_IsoOutIncomplete,
  USBD_TEMPLATE_GetCfgDesc,
  USBD_TEMPLATE_GetCfgDesc,
  USBD_TEMPLATE_GetCfgDesc,
  USBD_TEMPLATE_GetDeviceQualifierDesc,
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif /* __ICCARM__ */
/* USB TEMPLATE device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_TEMPLATE_CfgDesc[USB_CONFIG_DESC_SIZE] __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_CONFIG_DESC_SIZE, /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x04,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0x80,         /*bmAttributes: not bus powered and doesnt Supports Remote Wakeup (bm=bit map)*/
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus (in 2mA units: 50*2=100)*/

  /* Interface */
  0x09,                     /* bLength */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  DAP_V2_IF_NUM,            /* bInterfaceNumber */  //ToDo try 0 for interface according to polulu example
  0x00,                     /* bAlternateSetting */
  0x03,                     /* bNumEndpoints */
  0xFF,                     /* bInterfaceClass, vendor specific */
  0x00,                     /* bInterfaceSubClass */
  0x00,                     /* bInterfaceProtocol */
  0x05,                     /* iInterface */

  /* Endpoint OUT */
  0x07,                            	/* bLength */
  USB_DESC_TYPE_ENDPOINT,          	/* bDescriptorType */
  EPOUT_ADDR_SWD,             	 	/* bEndpointAddress */
  0x02,                            	/* bmAttributes: 02=bulk */
  LOBYTE(USB_HS_MAX_PACKET_SIZE),  	/* wMaxPacketSize */  //TODO is packet size correct?
  HIBYTE(USB_HS_MAX_PACKET_SIZE),
  0x00,                            	/* bInterval */

  /* Endpoint IN */
  0x07,                             /* bLength */
  USB_DESC_TYPE_ENDPOINT,           /* bDescriptorType */
  EPIN_ADDR_SWD,					/* bEndpointAddress */
  0x02,                             /* bmAttributes 02=bulk*/
  LOBYTE(USB_HS_MAX_PACKET_SIZE),   /* wMaxPacketSize */
  HIBYTE(USB_HS_MAX_PACKET_SIZE),
  0x00,                              /* bInterval */

  /* Endpoint IN */
  0x07,                             /* bLength */
  USB_DESC_TYPE_ENDPOINT,           /* bDescriptorType */
  EPIN_ADDR_SWO,         			/* bEndpointAddress */
  0x02,                             /* bmAttributes 02=bulk*/
  LOBYTE(USB_HS_MAX_PACKET_SIZE),   /* wMaxPacketSize */
  HIBYTE(USB_HS_MAX_PACKET_SIZE),
  0x00,                              /* bInterval */


  /**********  Descriptor of TEMPLATE interface 0 Alternate setting 0 **************/

};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif /* __ICCARM__ */
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_TEMPLATE_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};


/**
  * @}
  */

bool data_in_busy_EP_SWD = false;  //TO indicate if DataOut on sWD endpoint is ongoing
bool data_in_busy_EP_SWO = false;  //TO indicate if DataOut on SWO endpoint is ongoing

uint8_t bulk_interm_buf_SWD_EpOut[DAP_PACKET_SIZE]; //TODO figure out intermediate buffer sizes


/** @defgroup USBD_TEMPLATE_Private_Functions
  * @{
  */

/**
  * @brief  USBD_TEMPLATE_Init
  *         Initialize the TEMPLATE interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_TEMPLATE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{

  USBD_LL_OpenEP(pdev, EPOUT_ADDR_SWD, USBD_EP_TYPE_BULK, EPOUT_SIZE_SWD);
  USBD_LL_OpenEP(pdev, EPIN_ADDR_SWD, USBD_EP_TYPE_BULK, EPIN_SIZE_SWD);
  USBD_LL_OpenEP(pdev, EPIN_ADDR_SWO, USBD_EP_TYPE_BULK, EPIN_SIZE_SWO);
  USBD_LL_PrepareReceive(pdev, EPOUT_ADDR_SWD, bulk_interm_buf_SWD_EpOut, DAP_ID_PACKET_SIZE);


  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_Init
  *         DeInitialize the TEMPLATE layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_TEMPLATE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_LL_CloseEP(pdev, EPOUT_ADDR_SWD);
  USBD_LL_CloseEP(pdev, EPIN_ADDR_SWD);
  USBD_LL_CloseEP(pdev, EPIN_ADDR_SWO);
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_Setup
  *         Handle the TEMPLATE specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  *
  * @note   this is still setup phase, data is going trough ep0
  */
static uint8_t USBD_TEMPLATE_Setup(USBD_HandleTypeDef *pdev,
                                   USBD_SetupReqTypedef *req)
{
  USBD_StatusTypeDef ret = USBD_OK;

  uint16_t len = 0U;
  uint8_t *pbuf = NULL;
  uint8_t err = 0U;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest)
      {
        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;
    case USB_REQ_TYPE_VENDOR :
	  switch (req->bRequest)
	  {
#ifndef DAP_FW_V1
	    case USB_REQ_GET_MSOS2_STRDESC:
	    	printf("MS OS Str Descriptor\n\r");
	        pbuf = pdev->pDesc->GetMsOsStrDescriptor(pdev->dev_speed, &len);
	        break;
	      ret = USBD_OK;
	      break;
#endif /* (#ifndef DAP_FW_V1) */
		default:
		  USBD_CtlError(pdev, req);
		  ret = USBD_FAIL;
		  break;
	  }
	  break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  if (req->wLength != 0U)  //TODO cleanup, add proper retval
    {
      if (len != 0U)
      {
        len = MIN(len, req->wLength);
        (void)USBD_CtlSendData(pdev, pbuf, len);
      }
      else
      {
        USBD_CtlError(pdev, req);
      }
    }
    else
    {
      (void)USBD_CtlSendStatus(pdev);
    }

  return (uint8_t)ret;
}

/**
  * @brief  USBD_TEMPLATE_GetCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_TEMPLATE_GetCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_TEMPLATE_CfgDesc);
  return USBD_TEMPLATE_CfgDesc;
}

/**
  * @brief  USBD_TEMPLATE_GetDeviceQualifierDesc
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t *USBD_TEMPLATE_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_TEMPLATE_DeviceQualifierDesc);
  return USBD_TEMPLATE_DeviceQualifierDesc;
}

/**
  * @brief  USBD_TEMPLATE_DataIn
  *         handle data IN Stage.
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_TEMPLATE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  // A bulk transfer is complete when the endpoint does on of the following:
  // - Has transferred exactly the amount of data expected
  // - Transfers a packet with a payload size less than wMaxPacketSize or transfers a zero-length packet
  if (pdev->ep_in[epnum].total_length && !(pdev->ep_in[epnum].total_length % USB_HS_MAX_PACKET_SIZE))
  {
	pdev->ep_in[epnum].total_length = 0;
	USBD_LL_Transmit(pdev, epnum, NULL, 0);
  }
  else{
	if (epnum == (EPIN_ADDR_SWD & EP_ADDR_7F_MASK)){
		/* set flag to inidcate endpoint is free */
		data_in_busy_EP_SWD = false;

		/* Check if an other response is pending */
		uint8_t* buf = DAP_GetNexResponse();
		if (buf != NULL){
			USBD_TEMPLATE_Transmit_SWD(pdev,buf);
		}
	}

  	if (epnum == (EPIN_ADDR_SWO & EP_ADDR_7F_MASK)){
		data_in_busy_EP_SWO = false;
		//TODO add SWO transmit next?
	}
  }
  return USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_TEMPLATE_EP0_RxReady(USBD_HandleTypeDef *pdev)
{

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_TEMPLATE_EP0_TxReady(USBD_HandleTypeDef *pdev)
{

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_TEMPLATE_SOF(USBD_HandleTypeDef *pdev)
{

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_TEMPLATE_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_TEMPLATE_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_TEMPLATE_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_TEMPLATE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  uint32_t const bytes_received = USBD_LL_GetRxDataSize(pdev, epnum);
  if (bytes_received == 0){
	  while(1);
  }
  DAP_BulkSaveDataOut(bulk_interm_buf_SWD_EpOut, DAP_ID_PACKET_SIZE);    //TODO check if we have to copy entire block, or just bytes_received
  USBD_LL_PrepareReceive(pdev, EPOUT_ADDR_SWD, bulk_interm_buf_SWD_EpOut, DAP_ID_PACKET_SIZE);
  return USBD_OK;

  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_TEMPLATE_Transmit
  *         handle DataIn commands
  * @param  pdev: device instance
  * @param  buf: buffer containing the data
  * @param  length: number of bytes to be transmitted
  * @retval status
  *
  * @note This should be in an ...if.c file by ST convention.
  */
uint8_t  USBD_TEMPLATE_Transmit_SWD(USBD_HandleTypeDef *pdev, uint8_t* buf)
{

  if (data_in_busy_EP_SWD){
    return USBD_BUSY;
  }
  else{
	  data_in_busy_EP_SWD = true;
  	  pdev->ep_in[EPIN_ADDR_SWD & EP_ADDR_7F_MASK ].total_length = DAP_PACKET_SIZE; //TODO ezt a -1 dolgot mindnekepp tavolitsd el
  	  return USBD_LL_Transmit(pdev, EPIN_ADDR_SWD, buf, DAP_PACKET_SIZE);
  }
}

uint8_t  USBD_TEMPLATE_Transmit_SWO(USBD_HandleTypeDef *pdev, uint8_t* buf, uint16_t length)
{
  if (data_in_busy_EP_SWO)
    return USBD_BUSY;
  data_in_busy_EP_SWO = true;
  //TODO copy SWD data before trasmition. Not necessearly here
  pdev->ep_in[EPIN_ADDR_SWD & EP_ADDR_7F_MASK ].total_length = length;
  return USBD_LL_Transmit(pdev, EPIN_ADDR_SWO, buf, length);
}

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */
