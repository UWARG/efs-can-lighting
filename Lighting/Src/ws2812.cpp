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
}

WS2812::WS2812(uint8_t *output_buffer) {
	this->buffer = output_buffer;

	// TODO: replace these magic numbers
	this->g_offset = this->buffer;
	this->r_offset = this->buffer + 8;
	this->b_offset = this->buffer + 16;

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
	push_colour_to_output_buffer(this->colour);
}

void WS2812::set_led_colour(RGB_colour_t rgb_colour_value, uint8_t color_brightness) {

	this->colour = rgb_colour_value;
	this->brightness = color_brightness;


	RGB_colour_t rgb_compensated_colour = this->colour;

	float green = this->colour.green * ((float)color_brightness / 100.0);
	float red = this->colour.red * ((float)color_brightness / 100.0);
	float blue = this->colour.blue * ((float)color_brightness / 100.0);


	rgb_compensated_colour.green = green;
	rgb_compensated_colour.red = red;
	rgb_compensated_colour.blue = blue;

	push_colour_to_output_buffer(rgb_compensated_colour);
}


void WS2812::set_brightness(uint8_t color_brightness) {

	this->brightness = color_brightness;
	RGB_colour_t rgb_compensated_colour = this->colour;

	float green = this->colour.green * ((float)color_brightness / 100.0);
	float red = this->colour.red * ((float)color_brightness / 100.0);
	float blue = this->colour.blue * ((float)color_brightness / 100.0);

	push_colour_to_output_buffer(rgb_compensated_colour);
}



void WS2812::push_colour_to_output_buffer(RGB_colour_t color) {
	// TODO: remove magic numbers && properly explain this bit offset stuff
	for (int i = 0; i < bits_per_colour; ++i) {
		if ((color.red >> i) & 0x1) {
			this->r_offset[7 - i] = PWM_HI;
		} else {
			this->r_offset[7 - i] = PWM_LO;
		}

		if ((color.green >> i) & 0x1) {
			this->g_offset[7 - i] = PWM_HI;
		} else {
			this->g_offset[7 - i] = PWM_LO;
		}

		if ((color.blue >> i) & 0x1) {
			this->b_offset[7 - i] = PWM_HI;
		} else {
			this->b_offset[7 - i] = PWM_LO;
		}
	}
}




RGB_colour_t WS2812::get_led_colour() {
	return this->colour;
}
