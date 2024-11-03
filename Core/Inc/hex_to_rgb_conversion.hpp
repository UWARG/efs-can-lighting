/*
 * hex_to_rgb_conversion.h
 *
 *  Created on: Oct 29, 2024
 *      Author: mihirgupta
 */

#ifndef INC_HEX_TO_RGB_CONVERSION_HPP_
#define INC_HEX_TO_RGB_CONVERSION_HPP_


#include <stdint.h>

#define GREEN_BIT_MASK (0x00FF0000)
#define RED_BIT_MASK (0x0000FF00)
#define BLUE_BIT_MASK (0x000000FF)

//bits 8-15 of the color code form the red color byte
#define RED_SHIFT_INDEX 8
//bits 16-23 of the color code form the green color byte
#define GREEN_SHIFT_INDEX 16

uint8_t get_red_byte_from_hex(uint32_t hex_color_code);
uint8_t get_green_byte_from_hex(uint32_t hex_color_code);
uint8_t get_blue_byte_from_hex(uint32_t hex_color_code);


#endif /* INC_HEX_TO_RGB_CONVERSION_HPP_ */
