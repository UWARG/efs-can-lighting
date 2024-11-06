/*
 * ws2812.c
 *
 *  Created on: Sep 22, 2024
 *      Author: anthony
 */

#include "ws2812.hpp"
#include  "conversions.hpp"


uint8_t out_buf[BUFF_SIZE];


WS2812::WS2812() {
	reset_led_buffer();
}


void WS2812::reset_led_buffer() {
	for (int i = 0; i < BUFF_SIZE; i++) {
		//First and last 24 bytes LEDs get no valuable data so there's a gap between
		//data transfers.
		//For everything else, the buffer is filled with PWM_LO (for 0 brightness)
		//by default.
		if (i < 24 || i >= (NUM_LEDS + 1) * 24) {
			out_buf[i] = 0;
		} else {
			out_buf[i] = PWM_LO;
		}
	}
}

void WS2812::set_led_color_by_index(uint8_t index, uint8_t green_byte, uint8_t red_byte, uint8_t blue_byte) {
	uint8_t mask = 1;
	//For each color's brightness byte, mask off each bit 1 at a time.
	//If the color bit is a 1, write PWM_high to the appropriate index of the out_buf.
	//If the color bit is 0, write PWM_low.
	//for loop indices starting at 7 because the least significant bit gets mapped to the highest
	//output buffer index of all the color bytes.


	for (int g = 7; g >= 0; g--) {
		if (green_byte & mask) {
			//(index+1)*24 to skip to the part of the buffer that corresponds to LED #.
			out_buf[g + (index+1)*24] = PWM_HI;
		} else {
			out_buf[g + (index+1)*24] = PWM_LO;
		}
		mask = mask << 1;
	}
	//reset mask to analyze the next color byte.
	mask = 1;

	for (int r = 7; r >= 0; r--) {
		if (red_byte & mask) {
			out_buf[r + (index +1)*24 + 8] = PWM_HI;
		} else {
			out_buf[r + (index+1)*24 + 8] = PWM_LO;
		}
		mask = mask << 1;
	}

	mask = 1;

	for (int b = 7; b >= 0; b--) {
		if (blue_byte & mask) {
			out_buf[b + (index +1)*24 + 16] = PWM_HI;
		} else {
			out_buf[b + (index +1)*24 + 16] = PWM_LO;
		}
		mask = mask << 1;
	}
}

void WS2812::set_all_led_colors(uint8_t green_byte, uint8_t red_byte, uint8_t blue_byte) {
	for (int led = 0; led < NUM_LEDS; led++) {
		set_led_color_by_index(led, green_byte, red_byte, blue_byte);
	}
}


void WS2812::render_leds() {
	HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) out_buf, BUFF_SIZE);
}
