/*
 * ws2812.h
 *
 *  Created on: Sep 22, 2024
 *      Author: anni
 */

#ifndef INC_WS2812_H_
#define INC_WS2812_H_

#include <stdint.h>

// LED parameters
#define NUM_BPP (3) // 3 for WS2812B, 4 for SK6812
#define NUM_PIXELS (3)
#define NUM_BYTES (NUM_BPP * NUM_PIXELS)

// Function Declarations
void led_set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void led_set_RGBW(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_set_all_RGB(uint8_t r, uint8_t g, uint8_t b);
void led_set_all_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_render();

#endif /* INC_WS2812_H_ */
