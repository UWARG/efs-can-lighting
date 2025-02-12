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
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim;

// TODO: custom types?
static constexpr uint8_t NUM_LEDS = 10;
static constexpr uint8_t NUM_LEDS_PADDING = 19;
static constexpr uint16_t DMA_OUTPUT_BUFFER_SIZE = (NUM_LEDS
		+ NUM_LEDS_PADDING * 2) * 24 * 2;		// TODO: remove magic num
static constexpr uint16_t BANK_OUTPUT_BUFFER_SIZE = (NUM_LEDS
		+ NUM_LEDS_PADDING * 2) * 24 * 2;	// TODO: remove magic num

uint8_t dma_output_buffer[DMA_OUTPUT_BUFFER_SIZE];
uint8_t bank_output_buffer[BANK_OUTPUT_BUFFER_SIZE];

WS2812 leds[NUM_LEDS]; // TODO: make this work

RGB_colour_t WHITE = { 255, 255, 255 };
RGB_colour_t RED = { 255, 0, 0 };
RGB_colour_t ORANGE = {255, 165, 0};
RGB_colour_t GREEN = {0, 255, 0};
RGB_colour_t CYAN = {0, 255, 255};
RGB_colour_t BROWN = {139, 69, 19};
RGB_colour_t PURPLE = {255, 0, 255};

// Initial setup call
LightingController rev4(dma_output_buffer, bank_output_buffer, leds, &htim1, TIM_CHANNEL_2); // TODO: Once we have custom functions registered as callbacks.....

static volatile uint8_t g_input = 0;
static volatile uint8_t g_num_seconds = 0;

// Temporary (ish) function with exemplar code that allows us to test lighting board functionality without needing CAN commands
void run_lighting_board() {

	// Call to start lighting control
	rev4.start_lighting_control();

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
	uint8_t brightness = 5;

	// DOMAIN SETUP
	// TODO: move Control Domain building to special functions

	uint8_t BRIGHTNESS_MAX = 100;

	/* build standby domain
	 * Let's say the drone starts in standby for this demonstration.
	 */

	rev4.activate_domain(CD_BEACON);
	rev4.activate_domain(CD_STROBE);
	rev4.activate_domain(CD_STANDBY);
	rev4.activate_domain(CD_NAVIGATION);
	rev4.activate_domain(CD_BRAKE);
	rev4.activate_domain(CD_LANDING_TAKEOFF);
	rev4.activate_domain(CD_SEARCH);

	// allow all of our domains
	// comment any of these out to see the effect of allowing command domains
	rev4.allow_domain(CD_MAIN);
	rev4.allow_domain(CD_STANDBY);
	rev4.allow_domain(CD_BEACON);
	rev4.allow_domain(CD_STROBE);
	rev4.allow_domain(CD_LANDING_TAKEOFF);
	rev4.allow_domain(CD_NAVIGATION);
	rev4.allow_domain(CD_BRAKE);
	rev4.allow_domain(CD_SEARCH);

	while (true) {
		// Add a small delay for smooth transitions
		HAL_Delay(10); // Adjust this value for faster/slower fading

		rev4.set_domain_colour_and_brightness(CD_MAIN, my_colour, brightness);
		rev4.activate_domain(CD_MAIN);
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

	this->domain_leds_sets[CD_MAIN] = 255;
	this->domain_leds_sets[CD_STANDBY] = 352;
	this->domain_leds_sets[CD_BEACON] = 18;
	this->domain_leds_sets[CD_STROBE] = 45;
	this->domain_leds_sets[CD_LANDING_TAKEOFF] = 255;
	this->domain_leds_sets[CD_NAVIGATION] = 199;
	this->domain_leds_sets[CD_BRAKE] = 255;
	this->domain_leds_sets[CD_SEARCH] = 255;

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
					this->leds[j].set_led_colour(domain_colours[i],
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

void LightingController::enable_domain_leds(ControlDomain domain) {
	for (int i = 0; i < 16; i++) {
		if (this->domain_leds_sets[domain] & (1 << i)) {
			add_led_to_cd(i, domain);
		}
	}
}

void LightingController::disable_domain_leds(ControlDomain domain) {
	for (int i = 0; i < 16; i++) {
		if (this->domain_leds_sets[domain] & (1 << i)) {
			remove_led_from_cd(i, domain);
		}
	}
}



////////////////////
// STATE MACHINE FN
////////////////////

void LightingController::transition_to_search_state() {
	uint8_t num_top_facing_leds = 6;
	set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
	for (int i = 0; i < num_top_facing_leds; i++) {
		add_led_to_cd(i, CD_SEARCH);
	}
	this->drone_state = SEARCH_STATE;
}

void LightingController::transition_to(uint8_t input) {
	switch (this->drone_state) {
		case GROUND_STATE:
			if (input == 0) {												//go into search state
				remove_led_from_cd(0, CD_STANDBY);
				remove_led_from_cd(5, CD_STANDBY);
				remove_led_from_cd(6, CD_STANDBY);
				remove_led_from_cd(8, CD_STANDBY);
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {										//go into standby mode
				add_led_to_cd(0, CD_STANDBY);
				add_led_to_cd(5, CD_STANDBY);
				add_led_to_cd(6, CD_STANDBY);
				add_led_to_cd(8, CD_STANDBY);
				set_domain_colour_and_brightness(CD_STANDBY, ORANGE, 10);
			} else if (input == 2) {										//going into taxi state;
				remove_led_from_cd(0, CD_STANDBY);
				remove_led_from_cd(5, CD_STANDBY);
				remove_led_from_cd(6, CD_STANDBY);
				remove_led_from_cd(8, CD_STANDBY);

				add_led_to_cd(0, CD_STROBE);
				add_led_to_cd(2, CD_STROBE);
				add_led_to_cd(3, CD_STROBE);
				add_led_to_cd(5, CD_STROBE);

				add_led_to_cd(1, CD_BEACON);
				add_led_to_cd(4, CD_BEACON);

				set_domain_colour_and_brightness(CD_BEACON, RED, 15);
				set_domain_colour_and_brightness(CD_STROBE, WHITE, 4);
				this->drone_state = TAXI_STATE;
			}
			break;
		case TAXI_STATE:
			if (input == 0) {
				remove_led_from_cd(0, CD_STROBE);
				remove_led_from_cd(2, CD_STROBE);
				remove_led_from_cd(3, CD_STROBE);
				remove_led_from_cd(5, CD_STROBE);

				remove_led_from_cd(1, CD_BEACON);
				remove_led_from_cd(4, CD_BEACON);
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {									    //move to takeoff...
				remove_led_from_cd(0, CD_STROBE);
				remove_led_from_cd(2, CD_STROBE);
				remove_led_from_cd(3, CD_STROBE);
				remove_led_from_cd(5, CD_STROBE);

				remove_led_from_cd(1, CD_BEACON);
				remove_led_from_cd(4, CD_BEACON);

				for (int i = 0; i < 6; i++) {
					add_led_to_cd(i, CD_LANDING_TAKEOFF);
				}
				set_domain_colour_and_brightness(CD_LANDING_TAKEOFF, WHITE, 15);
				this->drone_state = TAKEOFF_STATE;
			}
			break;
		case TAKEOFF_STATE:
			if (input == 0) {
				for (int i = 0; i < 6; i++) {
					remove_led_from_cd(i, CD_LANDING_TAKEOFF);
				}
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) { 										//to landing_state;
				for (int i = 0; i < 6; i++) {
					remove_led_from_cd(i, CD_LANDING_TAKEOFF);
				}

				add_led_to_cd(0, CD_NAVIGATION);
				add_led_to_cd(1, CD_NAVIGATION);
				add_led_to_cd(2, CD_NAVIGATION);
				add_led_to_cd(6, CD_NAVIGATION);
				add_led_to_cd(7, CD_NAVIGATION);

				set_domain_colour_and_brightness(CD_NAVIGATION, RED, 30);
				this->drone_state = FLIGHT_STATE;
			} else if (input == 2) {
				this->drone_state = LANDING_STATE;
			}
			break;
		case FLIGHT_STATE:
			if (input == 0) {
				remove_led_from_cd(0, CD_NAVIGATION);
				remove_led_from_cd(1, CD_NAVIGATION);
				remove_led_from_cd(2, CD_NAVIGATION);
				remove_led_from_cd(6, CD_NAVIGATION);
				remove_led_from_cd(7, CD_NAVIGATION);

				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {										//move to landing state.
				remove_led_from_cd(0, CD_NAVIGATION);
				remove_led_from_cd(1, CD_NAVIGATION);
				remove_led_from_cd(2, CD_NAVIGATION);
				remove_led_from_cd(6, CD_NAVIGATION);
				remove_led_from_cd(7, CD_NAVIGATION);
				for (int i = 0; i < 6; i++) {
					add_led_to_cd(i, CD_LANDING_TAKEOFF);
				}
				set_domain_colour_and_brightness(CD_NAVIGATION, WHITE, 15);
				this->drone_state = LANDING_STATE;
			}
			break;
		case LANDING_STATE:
			if (input == 0) {
				for (int i = 0; i < 6; i++) {
					remove_led_from_cd(i, CD_LANDING_TAKEOFF);
				}
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {										//back to ground state in standby lighting mode
				for (int i = 0; i < 6; i++) {
					remove_led_from_cd(i, CD_LANDING_TAKEOFF);
				}
				add_led_to_cd(0, CD_STANDBY);
				add_led_to_cd(5, CD_STANDBY);
				add_led_to_cd(6, CD_STANDBY);
				add_led_to_cd(8, CD_STANDBY);
				this->drone_state = GROUND_STATE;
			} else if (input == 2) {										//to ground state but not in standby....
				for (int i = 0; i < 6; i++) {
					remove_led_from_cd(i, CD_LANDING_TAKEOFF);
				}
				this->drone_state = GROUND_STATE;

			} else if (input == 3) {										//to taxi state
				for (int i = 0; i < 6; i++) {
					remove_led_from_cd(i, CD_LANDING_TAKEOFF);
				}

				add_led_to_cd(0, CD_STROBE);
				add_led_to_cd(2, CD_STROBE);
				add_led_to_cd(3, CD_STROBE);
				add_led_to_cd(5, CD_STROBE);

				add_led_to_cd(1, CD_BEACON);
				add_led_to_cd(4, CD_BEACON);

				set_domain_colour_and_brightness(CD_BEACON, RED, 15);
				set_domain_colour_and_brightness(CD_STROBE, WHITE, 4);
				this->drone_state = TAXI_STATE;
			}
			break;
		case SEARCH_STATE:
			if (input == 0) {
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {

			} else if (input == 2) {

			} else if (input == 3) {

			} else if (input == 4) {

			} else if (input == 5) {

			}
			break;
		default:
			break;
	}
}

State LightingController::get_drone_state() {
	return this->drone_state;
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

	if (rev4.get_drone_state() == GROUND_STATE && g_num_seconds == 5) {
		rev4.transition_to(0);
		g_num_seconds = 0;
	} else if (rev4.get_drone_state() == TAXI_STATE && g_num_seconds == 5) {
		rev4.transition_to(1);
		rev4.activate_domain(CD_LANDING_TAKEOFF);
		g_num_seconds = 0;
	} else if(rev4.get_drone_state() == TAKEOFF_STATE && g_num_seconds == 5) {
		rev4.transition_to(1);
		rev4.activate_domain(CD_NAVIGATION);
		g_num_seconds = 0;
	} else if (rev4.get_drone_state() == FLIGHT_STATE && g_num_seconds == 5) {
		rev4.transition_to(1);
		rev4.activate_domain(CD_LANDING_TAKEOFF);
		g_num_seconds = 0;
	} else if (rev4.get_drone_state() == LANDING_STATE && g_num_seconds == 5) {
		rev4.transition_to(2);
		rev4.activate_domain(CD_STANDBY);
		g_num_seconds = 0;
	}

	g_num_seconds += 1;
}

void TIM7_100msCallback(TIM_HandleTypeDef *htim7) {
	static uint8_t stage = 0;
	static uint8_t led_index = 0;

	if (stage == 0) { 			// STROBE ON
		rev4.activate_domain(CD_STROBE);
		rev4.activate_domain(CD_STANDBY); //STANDBY ON;
		stage = 1;
	} else if (stage == 1) { 	// STROBE OFF
		rev4.deactivate_domain(CD_STROBE);
		rev4.deactivate_domain(CD_STANDBY);
		stage = 2;
	} else if (stage == 2) {	// STROBE ON
		rev4.activate_domain(CD_STROBE);
		rev4.activate_domain(CD_STANDBY); //STANDBY ON;
		stage = 3;
	} else if (stage == 3) {	// STROBE OFF
		rev4.deactivate_domain(CD_STROBE);
		rev4.deactivate_domain(CD_STANDBY);
		stage = 4;
	} else if (stage == 4) {	// OFF
		stage = 5;
	} else if (stage == 5) {	// OFF
		stage = 6;
	} else if (stage == 6) {	// BCN ON
		rev4.activate_domain(CD_BEACON);
		stage = 7;
	} else if (stage == 7) {	// BCN OFF
		rev4.deactivate_domain(CD_BEACON);
		stage = 0;
		HAL_TIM_Base_Stop_IT(htim7);
	}

	//"Scrolling" pattern for the "Search" drone state.

	if (rev4.get_drone_state() == SEARCH_STATE) {
		uint8_t num_top_facing_leds = 6;
		if (led_index == 0) {
			rev4.remove_led_from_cd(num_top_facing_leds-1, CD_SEARCH);
		} else {
			rev4.remove_led_from_cd(led_index-1, CD_SEARCH);
		}

		rev4.add_led_to_cd(led_index, CD_SEARCH);
		rev4.activate_domain(CD_SEARCH);
		led_index = (led_index + 1) % num_top_facing_leds;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		TIM6_OneSecondCallback(htim);
	} else if (htim->Instance == TIM7) {
		TIM7_100msCallback(htim);
	}
}
