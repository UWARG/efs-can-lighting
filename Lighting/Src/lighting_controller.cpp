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
static constexpr uint8_t NUM_LEDS = 2;			// Number of LED's on one board
static constexpr uint8_t NUM_LEDS_PADDING = 3;	// Number of LED's worth of padding (0) to give
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
//	initialize_bank_output_buffer_off(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING);
//	initialize_dma_output_buffer(dma_output_buffer, led_bank_output_buffer, LED_BANK_OUTPUT_BUFFER_SIZE);
//
//	HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) dma_output_buffer, DMA_OUTPUT_BUFFER_SIZE);

	int rotate_lo = 0;
	int rotate_hi = 8;

	uint8_t num_led = 3; // Number of LED's we have
	uint8_t led_in_cycle = 0;	// This is a value that we use to keep track of which LED is on during the cycle.
	num_led += 1; // +1 because we need 24 bits of 0 at the start
	uint8_t buff_base = num_led * 24; // Each LED has 24 Bits (8 for each of RGB)
	uint8_t out_buf[buff_base + 5600]; // We add sixteen to the end so that we have 24 (one LED) bits of 0
	uint8_t setval = PWM_LO;
int count = 0;
	while (true) {
		HAL_Delay(1000);
		initialize_bank_output_buffer_on(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING);
		HAL_Delay(1000);
		initialize_bank_output_buffer_off(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING);

		for (int i= 0; i < 24; i++) {
			out_buf[i] = 0; // set the fisrt 24 bits to 0
			for (int j=1; j < num_led; j++) {
			if ((i < rotate_hi) && (i >= rotate_lo) && ((i%8) >5)) {
				out_buf[i+j*24] = PWM_HI;
//				out_buf[i+24] = PWM_HI;
			} else {
				out_buf[i+j*24] = PWM_LO;
//				out_buf[i+24] = PWM_LO;
			}
			}
		}

		rotate_lo += 8;
		rotate_hi += 8;

		if (rotate_lo == 24) {
			rotate_lo = 0;
			rotate_hi = 8;
		}

		// No matter what operation we do, we set the last bits in the buffer to 0;
				for (int i = buff_base; i < buff_base + 5600; i++) { // TODO: Make this 24 a variable "zero_extend_length"
					out_buf[i] = 0;
				}

		//		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
		//		HAL_Delay(20);
				if (count == 1) {
					break;
				} else {
					HAL_Delay(500);
					count += 1;
				}
		//		HAL_Delay(100);
				break;
	}
	HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) out_buf,
							buff_base + 5600);
}

// CALLBACKS
// TODO: Register custom callbacks for the timers
// https://community.st.com/t5/stm32-mcus/how-to-use-register-callbacks-in-stm32/ta-p/580499
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//          ^ Current location
	// So update BANK 1
//	std::memcpy(dma_output_buffer, led_bank_output_buffer, LED_BANK_OUTPUT_BUFFER_SIZE);
//	initialize_bank_output_buffer_on(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING);
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//                   ^ Current location
	// So update BANK 2
//	std::memcpy(dma_output_buffer + LED_BANK_OUTPUT_BUFFER_SIZE, led_bank_output_buffer, LED_BANK_OUTPUT_BUFFER_SIZE);
//	initialize_bank_output_buffer_off(led_bank_output_buffer, NUM_LEDS, NUM_LEDS_PADDING);
//	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

