/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v1.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "debugger.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb custom HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_HS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 1 */
		  0x06,0x00,0xFF,         /*  Usage Page (vendor defined) ($FF00) global */
		  0x09,0x01,              /*  Usage (vendor defined) ($01) local */
		  0xA1,0x01,              /*  Collection (Application) */
		  0x15,0x00,              /*   LOGICAL_MINIMUM (0) */
		  0x25,0xFF,              /*   LOGICAL_MAXIMUM (255) */
		  0x75,0x08,              /*   REPORT_SIZE (8bit) */

		  // Input Report
		  0x96,0x00,0x04,         /*   Report Length (1024 REPORT_SIZE) */
//		  0x95,0x40,         	  /*   Report Length (64 REPORT_SIZE) */
		  0x09,0x01,              /*   USAGE (Vendor Usage 1) */
		  0x81,0x02,              /*   Input(data,var,absolute) */

		  // Output Report
		  0x96,0x00,0x04,         /*   Report Length (1024 REPORT_SIZE) */
//		  0x95,0x40,         	  /*   Report Length (64 REPORT_SIZE) */
		  0x09,0x01,              /*   USAGE (Vendor Usage 1) */
		  0x91,0x02,              /*   Output(data,var,absolute) */

		  // Feature Report
		  0x96,0x00,0x04,         /*   Report Length (1024 REPORT_SIZE) */
//		  0x95,0x40,         	  /*   Report Length (64 REPORT_SIZE) */
		  0x09,0x01,              /*   USAGE (Vendor Usage 1) */
		  0xB1,0x02,              /*   Feature(data,var,absolute) */
  /* USER CODE END 1 */
   0xC0    /*     END_COLLECTION             */
};
/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceHS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_HS(void);
static int8_t CUSTOM_HID_DeInit_HS(void);
static int8_t CUSTOM_HID_OutEvent_HS(void);
#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
static int8_t CUSTOM_HID_CtrlReqComplete_HS(uint8_t request, uint16_t wLength);
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
static int8_t CUSTOM_HID_InEvent_HS(void);

#ifdef USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED
static uint8_t *CUSTOM_HID_GetReport_HS(uint16_t *ReportLength);
#endif /* USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_HS =
{
  CUSTOM_HID_ReportDesc_HS,
  CUSTOM_HID_Init_HS,
  CUSTOM_HID_DeInit_HS,
  CUSTOM_HID_OutEvent_HS,
  CUSTOM_HID_InEvent_HS,
#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
  CUSTOM_HID_CtrlReqComplete_HS,
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
#ifdef USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED
  CUSTOM_HID_GetReport_HS,
#endif /* USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_HS(void)
{
  /* USER CODE BEGIN 8 */

  return (USBD_OK);
  /* USER CODE END 8 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_HS(void)
{
  /* USER CODE BEGIN 9 */
  return (USBD_OK);
  /* USER CODE END 9 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_HS(void)
{
  /* USER CODE BEGIN 10 */
  /* Start next USB packet transfer once data processing is completed */
  if (USBD_CUSTOM_HID_ReceivePacket(&hUsbDeviceHS) != (uint8_t)USBD_OK)
  {
    return -1;
  }

  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)hUsbDeviceHS.pClassDataCmsit[CUSTOMHID_InstID];

  /* OUTPUT REPORT was received, handle the data. */
  HID0_SetReport(HID_REPORT_OUTPUT, 0, 0, hhid->Report_buf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);

  return (USBD_OK);
}
  /* USER CODE END 10 */

#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
/**
  * @brief  CUSTOM_HID_CtrlReqComplete_HS
  *         Manage the CUSTOM HID control request complete
  * @param  request: control request
  * @param  wLength: request wLength
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_CtrlReqComplete_HS(uint8_t request, uint16_t wLength)
{
  UNUSED(wLength);

  switch (request)
  {
    case CUSTOM_HID_REQ_SET_REPORT:

      break;

    case CUSTOM_HID_REQ_GET_REPORT:

      break;

    default:
      break;
  }

  return (0);
}
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */

#ifdef USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED
/**
  * @brief  CUSTOM_HID_GetReport_HS
  *         Manage the CUSTOM HID control Get Report request
  * @param  event_idx: event index
  * @param  state: event state
  * @retval return pointer to HID report
  */
static uint8_t *CUSTOM_HID_GetReport_HS(uint16_t *ReportLength)
{
  UNUSED(ReportLength);
  uint8_t *pbuff;

  return (pbuff);
}
#endif /* USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */

/* USER CODE BEGIN 11 */

static int8_t CUSTOM_HID_InEvent_HS(void)
{
  /* USER CODE BEGIN extra */
  int32_t len;

  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)hUsbDeviceHS.pClassDataCmsit[CUSTOMHID_InstID];
  if ((len=HID0_GetReport(HID_REPORT_INPUT, USBD_HID_REQ_EP_INT, 0, hhid->Report_buf)) > 0)
  {
	  HID_Send_Report(&hUsbDeviceHS, hhid->Report_buf, len);
  }

  return (USBD_OK);
  /* USER CODE END extra */
}
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_HS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, report, len);
}
*/
/* USER CODE END 11 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

