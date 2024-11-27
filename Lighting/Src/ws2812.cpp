/*
 * ws2812.cpp
 *
 * The actual class definition for one node in the lighting board.
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni
 */

#include <cstdint>
#include <cstring>

#include "ws2812.hpp"

//
//// WS2812 class definition.
//// This is really just ONE led.
WS2812::WS2812() {
	cmd_mode = CM_MAIN;
}

WS2812::WS2812(uint8_t *output_buffer) {
	this->buffer = output_buffer;

	// TODO: replace these magic numbers
	this->g_offset = this->buffer;
	this->r_offset = this->buffer + 8;
	this->b_offset = this->buffer + 16;

	cmd_mode = CM_MAIN;
	initialize_led_off();

}

void WS2812::initialize_led_on() {
	this->colour.red = 255;
	this->colour.green = 255;
	this->colour.blue = 255;
	for (int i = 0; i < BITS_PER_LED; ++i) {
		if ((i % 8) > 3) {// TODO: remove this and offer a bitter way to do this
			// This is really only here to protect our eyes while we debug....
			this->buffer[i] = PWM_HI;
		} else {
			this->buffer[i] = PWM_LO;
		}
	}
}

void WS2812::initialize_led_on(uint8_t *led_output_buffer) {
	this->buffer = led_output_buffer;

	this->g_offset = this->buffer;
	this->r_offset = this->buffer + 8;
	this->b_offset = this->buffer + 16;

	initialize_led_on();
}

void WS2812::initialize_led_off() {
	this->colour.red = 0;
	this->colour.green = 0;
	this->colour.blue = 0;
	for (int i = 0; i < BITS_PER_LED; ++i) {
		this->buffer[i] = PWM_LO;
	}
}

void WS2812::initialize_led_off(uint8_t *led_output_buffer) {
	this->buffer = led_output_buffer;

	this->g_offset = this->buffer;
	this->r_offset = this->buffer + 8;
	this->b_offset = this->buffer + 16;

	initialize_led_off();
}

void WS2812::set_led_colour(RGB_colour_t rgb_colour_value) {
	this->colour = rgb_colour_value;

	// TODO: remove magic numbers && properly explain this bit offset stuff
	for (int i = 0; i < bits_per_colour; ++i) {
		if ((rgb_colour_value.red >> i) & 0x1) {
			this->r_offset[7 - i] = PWM_HI;
		} else {
			this->r_offset[7 - i] = PWM_LO;
		}

		if ((rgb_colour_value.green >> i) & 0x1) {
			this->g_offset[7 - i] = PWM_HI;
		} else {
			this->g_offset[7 - i] = PWM_LO;
		}

		if ((rgb_colour_value.blue >> i) & 0x1) {
			this->b_offset[7 - i] = PWM_HI;
		} else {
			this->b_offset[7 - i] = PWM_LO;
		}
	}
}

RGB_colour_t WS2812::get_led_colour() {
	return this->colour;
}
