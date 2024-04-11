/**
  ******************************************************************************
  * @file    usbd_template_core.h
  * @author  MCD Application Team
  * @brief   Header file for the usbd_template_core.c file.
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
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_TEMPLATE_CORE_H
#define __USB_TEMPLATE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"
#include "DAP_config.h"
/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_TEMPLATE
  * @brief This file is the header file for usbd_template_core.c
  * @{
  */


/** @defgroup USBD_TEMPLATE_Exported_Defines
  * @{
  */
#define DAP_V2_IF_NUM				0x00U
#define EPOUT_ADDR_SWD				0x01U
#define EPOUT_SIZE_SWD				USB_OTG_HS_MAX_PACKET_SIZE
#define EPIN_ADDR_SWD               0x81U
#define EPIN_SIZE_SWD               USB_OTG_HS_MAX_PACKET_SIZE
#define EPIN_ADDR_SWO				0x82U
#define EPIN_SIZE_SWO				0x10U   //TODO add SWO over bulk func.

#define USB_CONFIG_DESC_SIZE       	39U

/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */
extern USBD_ClassTypeDef USBD_TEMPLATE_ClassDriver;
extern uint8_t bulk_interm_buf_SWD_EpOut[DAP_PACKET_SIZE];



/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t  USBD_TEMPLATE_Transmit_SWD(USBD_HandleTypeDef *pdev, uint8_t* buf, uint32_t length);
uint8_t  USBD_TEMPLATE_Transmit_SWO(USBD_HandleTypeDef *pdev, uint8_t* buf, uint16_t length);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_TEMPLATE_CORE_H */
/**
  * @}
  */

/**
  * @}
  */
