/*
 * hex_to_rgb_conversion.c
 *
 *  Created on: Oct 29, 2024
 *      Author: mihirgupta
 */

#include "hex_to_rgb_conversion.h"

uint8_t get_red_from_hex(uint32_t hex_value) {
	return (hex_value & RED_BIT_MASK) >> 16;
}

uint8_t get_green_from_hex(uint32_t hex_value) {
	return (hex_value & GREEN_BIT_MASK) >> 8;
}

uint8_t get_blue_from_hex(uint32_t hex_value) {
	return (hex_value & BLUE_BIT_MASK);
}
