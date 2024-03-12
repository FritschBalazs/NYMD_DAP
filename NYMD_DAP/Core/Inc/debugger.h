/*
 * debugger.h
 *
 *  Created on: Jan 18, 2024
 *      Author: FBalazs
 */

#ifndef INC_DEBUGGER_H_
#define INC_DEBUGGER_H_

#include "main.h"
#include "usbd_customhid.h"


#define UART6_DEBUG  //UART 6, used for debugging proposes, CN4
//#define UART6_PASSTROUGH  //TODO uart6 passtrough

enum USBD_HID_REQ_t {
	USBD_HID_REQ_EP_CTRL = 0,
	USBD_HID_REQ_EP_INT,
	USBD_HID_REQ_PERIOD_UPDATE
};

enum USB_HID_REPORT_TYP_t {
	HID_REPORT_INPUT,
	HID_REPORT_FEATURE,
	HID_REPORT_OUTPUT,
};

int32_t HID0_GetReport (uint8_t rtype, uint8_t req, uint8_t rid, uint8_t *buf);
bool HID0_SetReport (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);
uint8_t HID_Send_Report(USBD_HandleTypeDef *pdev,uint8_t *report, uint16_t len);


void DAP_USB_Initialize (void);

void APP_Run(void);
void APP_Setup(void);

#endif /* INC_DEBUGGER_H_ */
