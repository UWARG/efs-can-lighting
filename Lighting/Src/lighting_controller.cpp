/*
 * lighting_controller.cpp
 *
 * This file handles all of the higher level logic / interfacing to the can board. This might be a class as well.
 *
 * It creates 6 instances of the WS2812 LED's
 *
 * It handles the final output buffer that is sent to DMA, as well as DMA transfer half complete callback
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni
 */

#include <cstring>
#include <stdint.h>

#include "tim.h"

#include "lighting_controller.hpp"
#include "ws2812.hpp"
#include "conversions.hpp"

// Use uint8_t instead of size_t to ensure we don't get too big
static constexpr uint8_t NUM_LEDS = 6;			// Number of LED's on one board
static constexpr uint8_t NUM_LEDS_PADDING = 6;	// Number of LED's worth of padding (0) to give
static constexpr uint16_t DMA_OUTPUT_BUFFER_SIZE = (NUM_LEDS + NUM_LEDS_PADDING*2) * 24 * 2;	// 24 bits per LED, (or padding), x2 for DMA functionality

// TODO: Replace this with something better???? Not sure what yet
static constexpr uint16_t LED_BANK_OUTPUT_BUFFER_SIZE = (NUM_LEDS + NUM_LEDS_PADDING*2) *24; // Padding for one bank of LED's, + padding

// TODO: define custom types so we don't need this weirdness
uint8_t dma_output_buffer[DMA_OUTPUT_BUFFER_SIZE];
uint8_t led_bank_output_buffer[LED_BANK_OUTPUT_BUFFER_SIZE];

extern TIM_HandleTypeDef htim;

void initialize_dma_output_buffer(uint8_t *dma_output_buffer, uint8_t *led_bank_output_buffer, uint16_t bank_size) {
	// Memcpy first bank
	std::memcpy(dma_output_buffer, led_bank_output_buffer, bank_size);
	// Memcpy second bank
	std::memcpy(dma_output_buffer + bank_size, led_bank_output_buffer, bank_size);
}

void run_lighting_board() {
	// initial setup
	initialize_bank_output_buffer_off(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING);
	initialize_dma_output_buffer(dma_output_buffer, led_bank_output_buffer, LED_BANK_OUTPUT_BUFFER_SIZE);

	// Start the Circular DMA buffer (once only)
	HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) dma_output_buffer, DMA_OUTPUT_BUFFER_SIZE);
	uint8_t temporary_led_brightness = 5;	// range from 0-8

	// FIRST LED starts when padding is done
	WS2812 led_0(led_bank_output_buffer + NUM_LEDS_PADDING*24 + 24*0);
	led_0.initialize_led_on();

	WS2812 led_1(led_bank_output_buffer + NUM_LEDS_PADDING*24 + 24*1);
	led_1.initialize_led_on();

	WS2812 led_3(led_bank_output_buffer + NUM_LEDS_PADDING*24 + 24*3);
	led_3.initialize_led_on();

	bool flip_flop = false;

	while (true) {
		// Update values of our bank_output_buffer as needed inside the while loop

		// For now, just flip/flop the top/outer edge led's
		if (flip_flop) {
			led_0.initialize_led_on();
			led_3.initialize_led_off();
		} else {
			led_0.initialize_led_off();
			led_3.initialize_led_on();
		}
		flip_flop = !flip_flop;

		HAL_Delay(1000);
		initialize_bank_output_buffer_on(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING, temporary_led_brightness);
		HAL_Delay(100);
		initialize_bank_output_buffer_off(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING);
		HAL_Delay(100);
		initialize_bank_output_buffer_on(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING, temporary_led_brightness);
		HAL_Delay(100);
		initialize_bank_output_buffer_off(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING);
	}
}

// CALLBACKS
// TODO: Register custom callbacks for the timers
// https://community.st.com/t5/stm32-mcus/how-to-use-register-callbacks-in-stm32/ta-p/580499
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//          ^ Current location
	// So update BANK 1
	std::memcpy(dma_output_buffer, led_bank_output_buffer, LED_BANK_OUTPUT_BUFFER_SIZE);
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//                   ^ Current location
	// So update BANK 2
	std::memcpy(dma_output_buffer + LED_BANK_OUTPUT_BUFFER_SIZE, led_bank_output_buffer, LED_BANK_OUTPUT_BUFFER_SIZE);
}

