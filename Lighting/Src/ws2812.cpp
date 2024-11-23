/*
 * ws2812.cpp
 *
 * The actual class definition for one node in the lighting board.
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni
 */

#include <stdint.h>

#include "ws2812.hpp"


// TODO: move buffer initialization to a different part of the code
void initialize_bank_output_buffer_off(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad) {
	uint16_t buffer_size = (num_led + 2*num_pad) * 24;
	for (int i = 0; i < buffer_size; ++i) {
		// if PAD
		if (i < 24*num_pad || i >= (buffer_size - 24*num_pad)) {
			bank_out_buff[i] = 0;
		} else {
			bank_out_buff[i] = PWM_LO;
		}
	}
}

void initialize_bank_output_buffer_on(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad) {
	uint16_t buffer_size = (num_led + 2*num_pad) * 24;
	for (int i = 0; i < buffer_size; i++) {
		// if PAD
		if (i < 24*num_pad || i >= (buffer_size - 24*num_pad)) {
			bank_out_buff[i] = 0;
		} else {
			bank_out_buff[i] = PWM_HI;
		}
	}
}

void initialize_bank_output_buffer_on(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad, uint8_t brightness) {
	uint16_t buffer_size = (num_led + 2*num_pad) * 24;
	for (int i = 0; i < buffer_size; i++) {
		// if PAD
		if (i < 24*num_pad || i >= (buffer_size - 24*num_pad)) {
			bank_out_buff[i] = 0;
		} else {
			if (i%8 > brightness) {
				bank_out_buff[i] = PWM_HI;
			} else {
				bank_out_buff[i] = PWM_LO;
			}
		}
	}
}
//
//// WS2812 class definition.
//// This is really just ONE led.
WS2812::WS2812(uint8_t *output_buffer) {
	this->buffer = output_buffer;
	initialize_led_off();

}
void WS2812::initialize_led_on() {
	for (int i = 0; i < BITS_PER_LED; i++) {
		this->buffer[i] = PWM_HI;
	}
}
void WS2812::initialize_led_off() {
	for (int i = 0; i < BITS_PER_LED; i++) {
		this->buffer[i] = PWM_LO;
	}
}

