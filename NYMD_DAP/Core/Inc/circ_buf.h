/*
 * circ_buf.h
 *
 *  Created on: Sep 4, 2023
 *      Author: frici
 */

#ifndef INC_CIRC_BUF_H_
#define INC_CIRC_BUF_H_

#include "main.h"

typedef struct _circ_buf_u8_t {
	uint16_t head;
	uint16_t tail;
	uint16_t max_size;
	uint8_t * data;
}circ_buf_u8_t;

extern circ_buf_u8_t printf_buf;


uint8_t cbuf_push_u8(circ_buf_u8_t* cb, uint8_t data_in);
uint8_t cbuf_pop_u8(circ_buf_u8_t* cb, uint8_t* data_out);
uint8_t cbuf_isFull_u8(circ_buf_u8_t* cb);
uint8_t cbuf_isEmpty_u8(circ_buf_u8_t* cb);
uint8_t cbuf_init_ALL();


#endif /* INC_CIRC_BUF_H_ */
