/*
 * ws2812.h
 *
 *  Created on: Sep 22, 2024
 *      Author: anni and Fola
 */

#ifndef INC_WS2812_H_
#define INC_WS2812_H_

#include <stdint.h>

#define NUM_LEDS 6

/*
 * Each LED receives a 24-bit frame of data. Buffer size large enough for 8 frames.
 * Please see led_buffer_init() function for the reason for the 2 extra frames.
 * 1 bit in a frame is mapped to 1 byte in the output buffer.
 */
#define BUFF_SIZE (NUM_LEDS + 2) * 24

extern uint8_t out_buf[BUFF_SIZE]; //Byte buffer of values to be transferred to the capture compare register (CCR).

// Constants
#define PWM_HI (38)
#define PWM_LO (19)

// Function Declarations
void led_buffer_init();
void led_set_RGB_index(uint8_t index, uint32_t color_code);
void led_set_RGB_cycle(uint32_t color_code);
void led_set_all_RGBs(uint32_t color_code);
void led_render_RGB();


#endif /* INC_WS2812_H_ */
