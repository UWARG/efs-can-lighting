/*
 * ws2812.h
 *
 *  Created on: Sep 22, 2024
 *      Author: anni and Fola
 *      Based on
 */

#ifndef INC_WS2812_HPP_
#define INC_WS2812_HPP_


#include <stdint.h>
#include "conversions.hpp"
#include "stm32l4xx_hal.h"
#include "can.hpp"
#include "dma.hpp"
#include "gpio.hpp"
#include "tim.hpp"

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
//void init_led_buffer();
//void set_led_color_by_index(uint8_t index, uint32_t color_code);
//void write_color_bytes_to_buffer(uint8_t index, uint8_t green_byte, uint8_t red_byte, uint8_t blue_byte, uint32_t color_code);
//void set_all_led_colors(uint32_t color_code);
//void render_led_colors();


class ws2812_LED {
public:
	ws2812_LED();

	void set_led_color_by_index(uint8_t index, uint32_t color_code);
	void set_all_led_colors(uint32_t color_code);
	void render_led_colors();

private:
	uint8_t out_buf[BUFF_SIZE];				//Byte buffer of values to be transferred to the capture compare register (CCR).
	TIM_HandleTypeDef hal_timer;				//we're using timer one, channel 2 for this application.
	HAL_StatusTypeDef status;

	void init_led_buffer();
	void write_color_bytes_to_buffer(uint8_t index, uint8_t green_byte, uint8_t red_byte, uint8_t blue_byte, uint32_t color_code);
};


//HAVE A SET color function.
//then overload that to take in either a RGB type (struct) or a hexcode.
//There are certain things that shouldn't be exposed to the user like init_led_buffer() and stuff. so just make sure...

#endif /* INC_WS2812_HPP_ */
