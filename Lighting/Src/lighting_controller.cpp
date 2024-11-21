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
static constexpr uint8_t NUM_LEDS = 1;			// Number of LED's on one board
static constexpr uint8_t NUM_LEDS_PADDING = 1;	// Number of LED's worth of padding (0) to give
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

	HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) dma_output_buffer, DMA_OUTPUT_BUFFER_SIZE);
	while (true) {

	}
}

// CALLBACKS
// TODO: Register custom callbacks for the timers
// https://community.st.com/t5/stm32-mcus/how-to-use-register-callbacks-in-stm32/ta-p/580499
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//          ^ Current location
	// So update BANK 1
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//                   ^ Current location
	// So update BANK 2

}

