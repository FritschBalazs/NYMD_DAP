/*
 * circ_buf.c
 *
 *  Created on: Sep 4, 2023
 *      Author: frici
 */

#include "main.h"
#include "circ_buf.h"


/* public buffer typedefs */
circ_buf_u8_t printf_buf;

/* private arrays for preallocated storage */
#define printf_buf_size (512U)
uint8_t printf_buf_data[printf_buf_size];

/* exported functions */
uint8_t cbuf_push_u8(circ_buf_u8_t* cb, uint8_t data_in);
uint8_t cbuf_pop_u8(circ_buf_u8_t* cb, uint8_t* data_out);
uint8_t cbuf_isFull_u8(circ_buf_u8_t* cb);
uint8_t cbuf_isEmpty_u8(circ_buf_u8_t* cb);

/* private functions */
uint8_t cbuf_init_u8(circ_buf_u8_t* cb, uint8_t* buf, uint16_t buf_len);
/* implementation */

uint8_t cbuf_init_ALL(){
	if (cbuf_init_u8(&printf_buf,printf_buf_data,printf_buf_size) == 0)
		return 0;
	else
		return 1;
}


uint8_t cbuf_push_u8(circ_buf_u8_t* cb, uint8_t data_in){
	if (cbuf_isFull_u8(cb)){
		return 1;
	}

	cb->data[cb->head++] = data_in;

	if (cb->head >= cb->max_size){
		cb->head = 0;
	}

	return 0;
}

uint8_t cbuf_pop_u8(circ_buf_u8_t* cb, uint8_t* data_out){
	if (cbuf_isEmpty_u8(cb)){
		return 1;
	}

	*data_out = cb->data[cb->tail++];

	if(cb->tail >= cb->max_size){
		cb->tail = 0;
	}

	return 0;

}
uint8_t cbuf_isFull_u8(circ_buf_u8_t* cb){
	if (cb->head+1 == cb->tail ){
		return 1;
	}
	else if (cb->head+1 == cb->max_size && cb->tail == 0){
		return 1;
	}

	return 0;
}
uint8_t cbuf_isEmpty_u8(circ_buf_u8_t* cb){
	if (cb->head == cb->tail){
		return 1;
	}
	return 0;
}


uint8_t cbuf_init_u8(circ_buf_u8_t* cb, uint8_t* buf, uint16_t buf_len){
	if (buf_len == 0 || buf == NULL){
		return 1;
	}

	cb->head = 0;
	cb->tail = 0;
	cb->max_size = buf_len;
	cb->data = buf;

	return 1;
}
