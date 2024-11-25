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

static constexpr uint8_t NUM_LEDS = 6;
static constexpr uint8_t NUM_LEDS_PADDING = 6;
static constexpr uint16_t DMA_OUTPUT_BUFFER_SIZE = (NUM_LEDS + NUM_LEDS_PADDING*2)*24*2;		// TODO: remove magic num
static constexpr uint16_t BANK_OUTPUT_BUFFER_SIZE = (NUM_LEDS + NUM_LEDS_PADDING*2)*24*2;	// TODO: remove magic num

uint8_t dma_output_buffer[DMA_OUTPUT_BUFFER_SIZE];
uint8_t bank_output_buffer[BANK_OUTPUT_BUFFER_SIZE];

WS2812 leds[NUM_LEDS]; // TODO: make this work

// TODO: define custom types so we don't need this weirdness
//uint8_t *dma_output_buffer;
//uint8_t *led_bank_output_buffer;
uint16_t LED_BANK_OUTPUT_BUFFER_SIZE = BANK_OUTPUT_BUFFER_SIZE;

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

	LightingController rev3(dma_output_buffer, bank_output_buffer, leds);

	rev3.start_lighting_control();
}

LightingController::LightingController(uint8_t *dma_output_buffer, uint8_t *bank_output_buffer, WS2812 *leds) {
	this->dma_buffer = dma_output_buffer;
	this->bank_buffer = bank_output_buffer;
	this->leds = leds;
	initialize_bank_buffer_on();
	initialize_dma_buffer();

	uint8_t* led_address;
	for (int i = 0; i < NUM_LEDS; i++) {
		led_address = bank_buffer + NUM_LEDS*24 + 24*i;
		this->leds[i].initialize_led_off(led_address);
	}
}

void LightingController::initialize_bank_buffer_off() {
	for (int i = 0; i < BANK_OUTPUT_BUFFER_SIZE; ++i) {
		// Check if the bit is a padding bit or value bit
		if (i < 24*NUM_LEDS_PADDING || i >= (BANK_OUTPUT_BUFFER_SIZE - 24*NUM_LEDS_PADDING)) {
			this->bank_buffer[i] = 0;
		} else {
			this->bank_buffer[i] = PWM_LO;
		}
	}
}

void LightingController::initialize_bank_buffer_on() {
	for (int i = 0; i < BANK_OUTPUT_BUFFER_SIZE; ++i) {
		// Check if the bit is a padding bit or value bit
		if (i < 24*NUM_LEDS_PADDING || i >= (BANK_OUTPUT_BUFFER_SIZE - 24*NUM_LEDS_PADDING)) {
			this->bank_buffer[i] = 0;
		} else {
			this->bank_buffer[i] = PWM_HI;
		}
	}
}

void LightingController::initialize_dma_buffer() {
	// memcpy first bank
	std::memcpy(this->dma_buffer, bank_output_buffer, BANK_OUTPUT_BUFFER_SIZE);
	// memcpy the second bank
	std::memcpy(this->dma_buffer + BANK_OUTPUT_BUFFER_SIZE, bank_output_buffer, BANK_OUTPUT_BUFFER_SIZE);
}

void LightingController::start_lighting_control() {
	HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) dma_output_buffer, DMA_OUTPUT_BUFFER_SIZE);

	// TODO: remove this testing code
	while (true) {
		initialize_bank_buffer_on();
		HAL_Delay(1000);
		initialize_bank_buffer_off();
		HAL_Delay(1000);
	}
}

// CALLBACKS
// TODO: Register custom callbacks for the timers
// https://community.st.com/t5/stm32-mcus/how-to-use-register-callbacks-in-stm32/ta-p/580499
// We should be able to register _class_ functions as callbacks?
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//          ^ Current location
	// So update BANK 1
	std::memcpy(dma_output_buffer, bank_output_buffer, BANK_OUTPUT_BUFFER_SIZE);
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//                   ^ Current location
	// So update BANK 2
	std::memcpy(dma_output_buffer + BANK_OUTPUT_BUFFER_SIZE, bank_output_buffer, BANK_OUTPUT_BUFFER_SIZE);
}
