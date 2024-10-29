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

#define NUM_LEDS 6
#define BUFF_SIZE (NUM_LEDS + 2) * 24 // Each LED has 24 Bits (8 for each of RGB)

extern uint8_t out_buf[BUFF_SIZE]; // We add 24 to the end so that we have 24 (one LED) bits of 0

// Constants
#define PWM_HI (38)
#define PWM_LO (19)

// Function Declarations
void led_set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void led_set_RGBW(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_set_all_RGB(uint8_t r, uint8_t g, uint8_t b);
void led_set_all_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_render();

void led_buffer_init();
void led_set_RGB_index(uint8_t index, uint32_t color_code);
void led_set_RGB_cycle(uint32_t color_code);
void led_set_all_RGBs(uint32_t color_code);
void led_render_RGB();
void led_render_cycle(uint32_t color_code);


#endif /* INC_WS2812_H_ */
