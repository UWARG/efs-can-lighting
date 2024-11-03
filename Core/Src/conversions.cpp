/*
 * conversions.c
 *
 *  Created on: Nov 2, 2024
 *      Author: folafatola and mihirgupta
 */

#include "conversions.hpp"

uint8_t get_red_byte_from_hex(uint32_t hex_color_code) {
	return (hex_color_code & RED_BIT_MASK) >> RED_SHIFT_INDEX;
}

uint8_t get_green_byte_from_hex(uint32_t hex_color_code) {
	return (hex_color_code & GREEN_BIT_MASK) >> GREEN_SHIFT_INDEX;
}

//No shift index because the first byte of the hex color code forms the blue color byte.
uint8_t get_blue_byte_from_hex(uint32_t hex_color_code) {
	return (hex_color_code & BLUE_BIT_MASK);
}
