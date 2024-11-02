/*
 * hex_to_rgb_conversion.c
 *
 *  Created on: Oct 29, 2024
 *      Author: mihirgupta
 */

#include "hex_to_rgb_conversion.h"

uint8_t get_red_byte_from_hex(uint32_t hex_value) {
	return (hex_value & RED_BIT_MASK) >> RED_SHIFT_INDEX;
}

uint8_t get_green_byte_from_hex(uint32_t hex_value) {
	return (hex_value & GREEN_BIT_MASK) >> GREEN_SHIFT_INDEX;
}

uint8_t get_blue_byte_from_hex(uint32_t hex_value) {
	return (hex_value & BLUE_BIT_MASK);
}
