/*
 * ws2812.h
 *
 *  Created on: Sep 22, 2024
 *      Author: anni
 */

#ifndef INC_WS2812_HPP_
#define INC_WS2812_HPP_

#include <stdint.h>
#include "stm32l4xx_hal.h"
#include "can.hpp"
#include "dma.hpp"
#include "tim.hpp"
#include "gpio.hpp"


// Constants
#define PWM_HI (38)
#define PWM_LO (19)


#define NUM_LEDS 6

/*
 * Each LED receives a 24-bit frame of data. Buffer size large enough for 8 frames.
 * Please see led_buffer_init() function for the reason for the 2 extra frames.
 * 1 bit in a frame is mapped to 1 byte in the output buffer.
 */
#define BUFF_SIZE (NUM_LEDS + 2) * 24  // We add 24 to the end so that we have 24 (one LED) bits of 0
extern uint8_t out_buf[BUFF_SIZE];

class WS2812 {

public:
	WS2812();
	void set_led_color_by_index(uint8_t index, uint8_t green_byte, uint8_t red_byte, uint8_t blue_byte);
	void set_all_led_colors(uint8_t green_byte, uint8_t red_byte, uint8_t blue_byte);
	void render_leds();

private:
	void reset_led_buffer();
	HAL_StatusTypeDef status;
};


#endif /* INC_WS2812_HPP_ */
