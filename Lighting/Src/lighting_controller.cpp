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
 *      Author: Anni and Fola
 */

#include <cstring>
#include <stdint.h>

#include "tim.h"

#include "lighting_controller.hpp"
#include "ws2812.hpp"
#include "conversions.hpp"

//#define STARTUP_SEQUENCE_1 // very basic selftest

extern TIM_HandleTypeDef htim7;

// TODO: custom types?
static constexpr uint8_t NUM_LEDS = 6;
static constexpr uint8_t NUM_LEDS_PADDING = 6;
static constexpr uint16_t DMA_OUTPUT_BUFFER_SIZE = (NUM_LEDS
		+ NUM_LEDS_PADDING * 2) * 24 * 2;		// TODO: remove magic num
static constexpr uint16_t BANK_OUTPUT_BUFFER_SIZE = (NUM_LEDS
		+ NUM_LEDS_PADDING * 2) * 24 * 2;	// TODO: remove magic num

uint8_t dma_output_buffer[DMA_OUTPUT_BUFFER_SIZE];
uint8_t bank_output_buffer[BANK_OUTPUT_BUFFER_SIZE];

WS2812 leds[NUM_LEDS]; // TODO: make this work

// Initial setup call
LightingController rev3(dma_output_buffer, bank_output_buffer, leds, &htim1, TIM_CHANNEL_2); // TODO: Once we have custom functions registered as callbacks.....

extern TIM_HandleTypeDef htim;

// Temporary (ish) function with exemplar code that allows us to test lighting board functionality without needing CAN commands
void run_lighting_board() {

	// Call to start lighting control
	rev3.start_lighting_control();

	// RGB Colour & Brightness control demo setup

	RGB_colour_t my_colour;
	my_colour.red = 65;
	my_colour.green = 0;
	my_colour.blue = 127;

	int red_direction = 1;   // 1 for increasing, -1 for decreasing
	int green_direction = 1; // 1 for increasing, -1 for decreasing
	int blue_direction = 1;  // 1 for increasing, -1 for decreasing

	uint8_t brightness_slow = 0;
	int brightness_direction = 1;
	uint8_t brightness = 50;

	// DOMAIN SETUP
	// TODO: move Control Domain building to special functions

	RGB_colour_t WHITE = { 255, 255, 255 };
	RGB_colour_t RED = { 255, 0, 0 };
	RGB_colour_t CYAN = {0, 255, 255};
	RGB_colour_t BROWN = {139, 69, 19};
	uint8_t BRIGHTNESS_MAX = 100;

	// build beacon domain
	rev3.set_domain_colour(CD_BEACON, CYAN);
	rev3.set_domain_brightness(CD_BEACON, 255);
	rev3.add_led_to_cd(1, CD_BEACON);
	rev3.add_led_to_cd(4, CD_BEACON);

	// build strobe domain
	// comment any of these out to see the effect of adding LED's
	rev3.set_domain_colour(CD_STROBE, BROWN);
	rev3.set_domain_brightness(CD_STROBE, 127);
	rev3.add_led_to_cd(0, CD_STROBE);
	rev3.add_led_to_cd(2, CD_STROBE);
	rev3.add_led_to_cd(3, CD_STROBE);
	rev3.add_led_to_cd(5, CD_STROBE);

	// allow all of our domains
	// comment any of these out to see the effect of allowing command domains
	rev3.allow_domain(CD_MAIN);
	rev3.allow_domain(CD_BEACON);
	rev3.allow_domain(CD_STROBE);


	while (true) {
		// Demo program to update LED colors & show control domain functionality

		brightness_slow = brightness_slow + 1;
		if (brightness_slow >= 20) {
			brightness += brightness_direction;
			brightness_slow = 0;
		}
		if (brightness >= 50) {
			brightness = 50;
			brightness_direction = -1;
		} else if (brightness <= 0) {
			brightness = 0;
			brightness_direction = 1;
		}

		// Update red value
		my_colour.red += red_direction;
		if (my_colour.red >= 255) {
			my_colour.red = 255;
			red_direction = -1; // Start decreasing
		} else if (my_colour.red <= 0) {
			my_colour.red = 0;
			red_direction = 1; // Start increasing
		}

		// Update green value
		my_colour.green += green_direction;
		if (my_colour.green >= 255) {
			my_colour.green = 255;
			green_direction = -1; // Start decreasing
		} else if (my_colour.green <= 0) {
			my_colour.green = 0;
			green_direction = 1; // Start increasing
		}

		// Update blue value
		my_colour.blue += blue_direction;
		if (my_colour.blue >= 255) {
			my_colour.blue = 255;
			blue_direction = -1; // Start decreasing
		} else if (my_colour.blue <= 0) {
			my_colour.blue = 0;
			blue_direction = 1; // Start increasing
		}

		// Add a small delay for smooth transitions
		HAL_Delay(10); // Adjust this value for faster/slower fading

		rev3.set_domain_colour_and_brightness(CD_MAIN, my_colour, brightness);
		rev3.activate_domain(CD_MAIN);
	}
}

LightingController::LightingController(uint8_t *dma_output_buffer,
		uint8_t *bank_output_buffer, WS2812 *leds, TIM_HandleTypeDef *timer, uint16_t timer_channel) {
	this->dma_buffer = dma_output_buffer;
	this->bank_buffer = bank_output_buffer;
	this->leds = leds;
	this->htimx = timer;
	this->tim_channel_x = timer_channel;
	initialize_bank_buffer_on();
	initialize_dma_buffer();

	// Initialize all of the internal LED's as well
	for (int i = 0; i < NUM_LEDS; ++i) {
		this->leds[i].initialize_led_off(
				bank_output_buffer + NUM_LEDS_PADDING * 24 + 24 * i);
		this->domain_leds[CD_MAIN] |= 1 << i;	// this led index is now enabled
	}
}

void LightingController::start_lighting_control() {
	HAL_TIMEx_PWMN_Start_DMA(this->htimx, this->tim_channel_x,
			(uint32_t*) dma_output_buffer, DMA_OUTPUT_BUFFER_SIZE);

#ifdef STARTUP_SEQUENCE_1
	HAL_Delay(1000);
	// Average startup selftest moment
	for (int i = 0; i < NUM_LEDS; ++i) {
		this->leds[i].initialize_led_off();
		HAL_Delay(1000);
	}

	for (int i = 0; i < NUM_LEDS; ++i) {
		RGB_colour_t my_colour;
		my_colour.red = (i % 3 == 0) ? 80 : 0;
		my_colour.green = (i % 3 == 1) ? 80 : 0;
		my_colour.blue = (i % 3 == 2) ? 80 : 0;
		this->leds[i].set_led_colour(my_colour);
		HAL_Delay(1000);
	}
#endif
}

void LightingController::recolour_all(RGB_colour_t desired_colour) {
	for (int i = 0; i < NUM_LEDS; ++i) {
		this->leds[i].set_led_colour(desired_colour);
	}
}

void LightingController::recolour_all(RGB_colour_t desired_colour,
		uint8_t brightness) {
	for (int i = 0; i < NUM_LEDS; ++i) {
		this->leds[i].set_led_colour(desired_colour, brightness);
	}
}

void LightingController::recolour_by_index(uint8_t index,
		RGB_colour_t desired_colour) {
	this->leds[index].set_led_colour(desired_colour);
}

void LightingController::recolour_by_index(uint8_t index,
		RGB_colour_t desired_colour, uint8_t brightness) {
	this->leds[index].set_led_colour(desired_colour, brightness);
}

////////////////////
// CONTROL DOMAIN FN
////////////////////

void LightingController::add_led_to_cd(uint8_t index, ControlDomain domain) {
	// set the index in the domain
	this->domain_leds[domain] |= 1 << index;
}

void LightingController::remove_led_from_cd(uint8_t index,
		ControlDomain domain) {
	// clear the index from the domain
	this->domain_leds[domain] &= ~(1 << index);
}

void LightingController::set_domain_colour_and_brightness(ControlDomain domain,
		RGB_colour_t colour, uint8_t brightness) {
	this->domain_colours[domain] = colour;
	this->domain_brightness[domain] = brightness;
}


void LightingController::set_domain_brightness(ControlDomain domain, uint8_t brightness) {
	this->domain_brightness[domain] = brightness;
}

void LightingController::set_domain_colour(ControlDomain domain, RGB_colour_t colour) {
	this->domain_colours[domain] = colour;
}

void LightingController::activate_domain(ControlDomain domain) {
	if (this->domain_allowed & (1 << domain)) {
		this->domain_active |= 1 << domain;	// TODO: have active/disabled booleans
		for (int i = 0; i < CD_LENGTH; ++i) {
			if (this->domain_active & (1 << i)) { // IF THIS DOMAIN IS ACTIVE
				for (int j = 0; j < NUM_LEDS; ++j) {
					if (this->domain_leds[i] & (1 << j)) {
						this->leds[j].set_led_colour(domain_colours[i],
								domain_brightness[i]);
					}
				}
			}
		}
	} else {
		// Domains should not be active when they are disallowed.
		// Instead of checking this every time we poll domains, check it on domain activation request.
		this->domain_active &= ~(1 << domain);
	}
}

void LightingController::deactivate_domain(ControlDomain domain) {
	this->domain_active &= ~(1 << domain);
	for (int i = 0; i < CD_LENGTH; ++i) {
		if (this->domain_active & (1 << i)) { // IF THIS DOMAIN IS ACTIVE
			for (int j = 0; j < NUM_LEDS; ++j) {
				if (this->domain_leds[i] & (1 << j)) {
					this->leds[i].set_led_colour(domain_colours[i],
							domain_brightness[i]);
				}
			}
		}
	}
}

void LightingController::allow_domain(ControlDomain domain) {
	this->domain_allowed |= 1 << domain;
}

void LightingController::disallow_domain(ControlDomain domain) {
	this->domain_allowed &= ~(1 << domain);
}

/////////////////
// Private fn
/////////////////
void LightingController::initialize_bank_buffer_off() {
	for (int i = 0; i < BANK_OUTPUT_BUFFER_SIZE; ++i) {
		// Check if the bit is a padding bit or value bit
		if (i < 24 * NUM_LEDS_PADDING
				|| i >= (BANK_OUTPUT_BUFFER_SIZE - 24 * NUM_LEDS_PADDING)) {
			this->bank_buffer[i] = 0;
		} else {
			this->bank_buffer[i] = PWM_LO;
		}
	}
}

void LightingController::initialize_bank_buffer_on() {
	for (int i = 0; i < BANK_OUTPUT_BUFFER_SIZE; ++i) {
		// Check if the bit is a padding bit or value bit
		if (i < 24 * NUM_LEDS_PADDING
				|| i >= (BANK_OUTPUT_BUFFER_SIZE - 24 * NUM_LEDS_PADDING)) {
			this->bank_buffer[i] = 0;
		} else {
			if ((i % 8) > 4) {
				this->bank_buffer[i] = PWM_HI;
			} else {
				this->bank_buffer[i] = PWM_LO;
			}
		}
	}
}

void LightingController::initialize_dma_buffer() {
	// memcpy first bank
	std::memcpy(this->dma_buffer, bank_output_buffer, BANK_OUTPUT_BUFFER_SIZE);
	// memcpy the second bank
	std::memcpy(this->dma_buffer + BANK_OUTPUT_BUFFER_SIZE, bank_output_buffer,
			BANK_OUTPUT_BUFFER_SIZE);
}

//////////////////////////////////////////
// CALLBACKS
//////////////////////////////////////////
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
	std::memcpy(dma_output_buffer + BANK_OUTPUT_BUFFER_SIZE, bank_output_buffer,
			BANK_OUTPUT_BUFFER_SIZE);
}

void TIM6_OneSecondCallback(TIM_HandleTypeDef *htim) {
	HAL_TIM_Base_Start_IT(&htim7);
}

void TIM7_100msCallback(TIM_HandleTypeDef *htim7) {
	static uint8_t stage = 0;
	if (stage == 0) { 			// STROBE ON
		rev3.activate_domain(CD_STROBE);
		stage = 1;
	} else if (stage == 1) { 	// STROBE OFF
		rev3.deactivate_domain(CD_STROBE);
		stage = 2;
	} else if (stage == 2) {	// STROBE ON
		rev3.activate_domain(CD_STROBE);
		stage = 3;
	} else if (stage == 3) {	// STROBE OFF
		rev3.deactivate_domain(CD_STROBE);
		stage = 4;
	} else if (stage == 4) {	// OFF
		stage = 5;
	} else if (stage == 5) {	// OFF
		stage = 6;
	} else if (stage == 6) {	// BCN ON
		rev3.activate_domain(CD_BEACON);
		stage = 7;
	} else if (stage == 7) {	// BCN OFF
		rev3.deactivate_domain(CD_BEACON);
		stage = 0;
		HAL_TIM_Base_Stop_IT(htim7);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		TIM6_OneSecondCallback(htim);
	} else if (htim->Instance == TIM7) {
		TIM7_100msCallback(htim);
	}
}
