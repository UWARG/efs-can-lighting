/*
 * ws2812.c
 *
 *  Created on: Sep 22, 2024
 *      Author: Anthony Luo and Fola Fatola
 *
 *      Reference:
 *      https://www.thevfdcollective.com/blog/stm32-and-sk6812-rgbw-led
 */

#include <hex_to_rgb_conversion.hpp>
#include <ws2812.hpp>
#include "stm32l4xx_hal.h"

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_ch2;

uint8_t out_buf[BUFF_SIZE]; // We add 24 to the end so that we have 24 (one LED) bits of 0

void init_led_buffer() {
	for (int i = 0; i < BUFF_SIZE; i++) {
		//First and last 24 bytes LEDs get no valuable data so there's a gap between
		//data transfers.
		//For everything else, the buffer is filled with PWM_LO (for 0 brightness)
		//by default.
		if (i < 24 || i > (NUM_LEDS + 1) * 24) {
			out_buf[i] = 0;
		} else {
			out_buf[i] = PWM_LO;
		}
	}
}

void write_color_bytes_to_buffer(uint8_t index, uint8_t green_byte, uint8_t red_byte, uint8_t blue_byte, uint32_t color_code) {

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

//Writes the color specified by "color_code" to the neopixel specified by the index.
void set_led_color_by_index(uint8_t index, uint32_t color_code) {

	//Get the brightness of each color
	uint8_t green_brightness_byte = get_green_byte_from_hex(color_code);
	uint8_t red_brightness_byte = get_red_byte_from_hex(color_code);
	uint8_t blue_brightness_byte = get_blue_byte_from_hex(color_code);

	write_color_bytes_to_buffer(index, green_brightness_byte, red_brightness_byte, blue_brightness_byte, color_code);
}


//set the color for all 6 neopixels
void set_all_led_colors(uint32_t color_code) {
	for (int i = 0; i < NUM_LEDS; i++) {
		set_led_color_by_index(i, color_code);
	}
}

//Display the colors.
void render_led_colors() {
	HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) out_buf,
					BUFF_SIZE);
}

//TODO: Global flag for if DMA transfer has finished?
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {

}