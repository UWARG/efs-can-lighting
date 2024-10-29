/*
 * hex_to_rgb_conversion.h
 *
 *  Created on: Oct 29, 2024
 *      Author: mihirgupta
 */

#ifndef INC_HEX_TO_RGB_CONVERSION_H_
#define INC_HEX_TO_RGB_CONVERSION_H_


#include <stdint.h>

#define RED_BIT_MASK (0x00FF0000)
#define GREEN_BIT_MASK (0x0000FF00)
#define BLUE_BIT_MASK (0x000000FF)

uint8_t get_red_from_hex(uint32_t hex_value);
uint8_t get_green_from_hex(uint32_t hex_value);
uint8_t get_blue_from_hex(uint32_t hex_value);


#endif /* INC_HEX_TO_RGB_CONVERSION_H_ */
