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

static constexpr uint8_t PWM_LO = 19;
static constexpr uint8_t PWM_HI = 38;

void initialize_bank_output_buffer_off(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad) {
	uint16_t buffer_size = (num_led + 2*num_pad) * 24;
	for (int i = 0; i < buffer_size; ++i) {
		// if PAD
		if (i < 24*num_pad || i >= buffer_size - 24*num_pad) {
			bank_out_buff[i] = 0;
		} else {
			bank_out_buff[i] = PWM_LO;
		}
	}
}

void initialize_bank_output_buffer_on(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad) {
	uint16_t buffer_size = (num_led + 2*num_pad) * 24;
	for (int i = 0; i < buffer_size; ++i) {
		// if PAD
		if (i < 24*num_pad || i >= buffer_size - 24*num_pad) {
			bank_out_buff[i] = 0;
		} else {
			bank_out_buff[i] = PWM_HI;
		}
	}
}

