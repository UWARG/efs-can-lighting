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

//list of colours
RGB_colour_t WHITE = { 255, 255, 255 };
RGB_colour_t RED = { 255, 0, 0 };
RGB_colour_t ORANGE = {255, 165, 0};
RGB_colour_t GREEN = {0, 255, 0};
RGB_colour_t CYAN = {0, 255, 255};
RGB_colour_t BROWN = {139, 69, 19};
RGB_colour_t PURPLE = {255, 0, 255};

// Initial setup call
LightingController rev4(dma_output_buffer, bank_output_buffer, leds, &htim1, TIM_CHANNEL_2); // TODO: Once we have custom functions registered as callbacks.....

// Temporary (ish) function with exemplar code that allows us to test lighting board functionality without needing CAN commands
void run_lighting_board() {

	// Call to start lighting control
	rev4.start_lighting_control();
	HAL_TIM_Base_Start_IT(&htim2);

	// DOMAIN SETUP
	// TODO: move Control Domain building to special functions

	// allow all of our domains
	// comment any of these out to see the effect of allowing command domains
	rev4.allow_domain(CD_MAIN);
	rev4.allow_domain(CD_TAXI);
	rev4.allow_domain(CD_BEACON);
	rev4.allow_domain(CD_STROBE);
	rev4.allow_domain(CD_LANDING);
	rev4.allow_domain(CD_NAVIGATION);
	rev4.allow_domain(CD_BRAKE);
	rev4.allow_domain(CD_SEARCH);

	rev4.activate_domain(CD_MAIN);
	while (true) {

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

	//enable beacon LEDs at the start to show that the drone is on
	uint8_t beacon_leds = 1 + (1 << 2) + (1 << 3) + (1 << 5) + (1 << 6) + (1 << 8);
	add_leds_to_cd(CD_BEACON, beacon_leds);
	rev4.set_domain_colour_and_brightness(CD_BEACON, RED, 15);
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


void LightingController::add_leds_to_cd(ControlDomain domain, uint16_t leds) {
	for (int i = 0; i < NUM_LEDS; ++i) {
		if (leds & (1 << i)) {				//if this LED is one I want to add to the control domain.
			add_led_to_cd(i, domain);
		}
	}
}

void LightingController::remove_leds_from_cd(ControlDomain domain, uint16_t leds) {
	for (int i = 0; i < NUM_LEDS; ++i) {
		if (leds & (1 << i)) {				//if this LED is one I want to remove from the control domain.
			remove_led_from_cd(i, domain);
		}
	}
}

////////////////////
// STATE MACHINE FN
////////////////////
void LightingController::transition_to(uint8_t input) {
	uint16_t beacon_leds = 0;
	uint16_t taxi_leds = (1 << 7) + (1 << 9);
	uint16_t strobe_leds = 0;
	uint16_t navigation_leds = 0;
	uint16_t landing_leds = (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4);
	uint8_t search_leds = 63; //leds 0 to 5

	switch (this->drone_state) {
		case GROUND_STATE:
			if (input == 0) {												//go into search state
				//disable ALL possibly active beacon and strobe lights.
				beacon_leds = 63;
				strobe_leds = 56;
				remove_leds_from_cd(CD_BEACON, beacon_leds);
				remove_leds_from_cd(CD_BEACON, strobe_leds);

				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				HAL_TIM_Base_Stop_IT(&htim2);								//start breathing.
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {										//go into standby mode
				//switch out beacon leds and enable strobe lights for standby.
				beacon_leds = 1 + (1 << 2) + (1 << 3) + (1 << 5);
				strobe_leds = beacon_leds;
				remove_leds_from_cd(CD_BEACON, beacon_leds);
				beacon_leds = (1 << 1) + (1 << 4);
				add_leds_to_cd(CD_BEACON, beacon_leds);
				set_domain_colour(CD_STROBE, RED);

				add_leds_to_cd(CD_STROBE, strobe_leds);
				set_domain_colour_and_brightness(CD_STROBE, ORANGE, 15);
			} else if (input == 2) {										//going into taxi state;
				//disable all downwards-facing beacon leds and remove the strobe lights used for standby.
				beacon_leds = 63;
				strobe_leds = 1 + (1 << 2) + (1 << 3) + (1 << 5);
				remove_leds_from_cd(CD_BEACON, beacon_leds);
				remove_leds_from_cd(CD_STROBE, strobe_leds);

				add_leds_to_cd(CD_TAXI, taxi_leds);
				set_domain_colour_and_brightness(CD_TAXI, WHITE, 10);
				this->drone_state = TAXI_STATE;
			}
			break;
		case TAXI_STATE:
			if (input == 0) {	//remove landing LEDS and set up search.
				remove_leds_from_cd(CD_TAXI, taxi_leds);
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {										//go into take-off state
				//remove_taxi_leds
				remove_leds_from_cd(CD_TAXI, taxi_leds);
				strobe_leds = (1 << 7) + (1 << 9);

				//enable strobe LEDs and allow them to be bright for take-off.
				add_leds_to_cd(CD_STROBE, strobe_leds);
				set_domain_colour_and_brightness(CD_STROBE, WHITE, 30);
				HAL_TIM_Base_Stop_IT(&htim2);
				this->drone_state = TAKE_OFF_STATE;
			}
			break;
		case TAKE_OFF_STATE:
			if (input == 0) {
				remove_leds_from_cd(CD_STROBE, strobe_leds);
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) { 										//go to flight state
				//keep the strobe lights while flying??
				navigation_leds = (1 << 3) + (1 << 4) + (1 << 5);
				add_leds_to_cd(CD_NAVIGATION, navigation_leds);
				set_domain_colour_and_brightness(CD_NAVIGATION, GREEN, 15);
				set_domain_brightness(CD_MAIN, 0);
				this->drone_state = FLIGHT_STATE;
			} else if (input == 2) {
				this->drone_state = LANDING_STATE;
			}
			break;
		case FLIGHT_STATE:
			if (input == 0) {												//go search.
				//no anti-collision strobe?
				strobe_leds = (1 << 7) + (1 << 9);
				remove_leds_from_cd(CD_STROBE, strobe_leds);

				//no more nav lights.
				navigation_leds = (1 << 3) + (1 << 4) + (1 << 5);
				remove_leds_from_cd(CD_NAVIGATION, navigation_leds);

				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				set_domain_brightness(CD_MAIN, 5);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {										//move to landing state.
				//move nav lights on the downwards-facing board to the top of the board.
				navigation_leds = (1 << 3) + (1 << 4) + (1 << 5);
				remove_leds_from_cd(CD_NAVIGATION, navigation_leds);
				navigation_leds = 1 + (1 << 5);
				add_leds_to_cd(CD_NAVIGATION, navigation_leds);
				set_domain_colour_and_brightness(CD_NAVIGATION, GREEN, 15);

				//enable landing lights.
				add_leds_to_cd(CD_LANDING, landing_leds);
				set_domain_colour_and_brightness(CD_LANDING, WHITE, 15);

				set_domain_brightness(CD_MAIN, 5);
				this->drone_state = LANDING_STATE;
			}
			break;
		case LANDING_STATE:
			navigation_leds = 1 + (1 << 5);
			strobe_leds = (1 << 7) + (1 << 9);
			if (input == 0) {												//enter SEARCH_STATE
				//no anti-collision strobe?
				remove_leds_from_cd(CD_STROBE, strobe_leds);
				remove_leds_from_cd(CD_LANDING, landing_leds);
				remove_leds_from_cd(CD_NAVIGATION, navigation_leds);
				beacon_leds = (1 << 1) + (1 << 4);
				remove_leds_from_cd(CD_BEACON, beacon_leds);
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = SEARCH_STATE;
			} else if (input == 1) {										//back to ground state in standby lighting mode
				this->drone_state = GROUND_STATE;
			} else if (input == 2) {										//to ground state but not in standby....
				//no anti-collision strobe?
				remove_leds_from_cd(CD_STROBE, strobe_leds);
				remove_leds_from_cd(CD_LANDING, landing_leds);

				//remove beacon lights
				beacon_leds = (1 << 1) + (1 << 4);
				remove_leds_from_cd(CD_BEACON, beacon_leds);
				beacon_leds = 1 + (1 << 2) + (1 << 3) + (1 << 5);			//add ground-facing beacon lights
				add_leds_to_cd(CD_BEACON, beacon_leds);

				HAL_TIM_Base_Start_IT(&htim2);
				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = GROUND_STATE;
			} else if (input == 3) {										//to taxi state
				remove_leds_from_cd(CD_LANDING, landing_leds);
				add_leds_to_cd(CD_TAXI, taxi_leds);
				set_domain_colour_and_brightness(CD_TAXI, WHITE, 10);
				this->drone_state = TAXI_STATE;
			}
			break;
		case SEARCH_STATE:
			if (input == 0) {												//back to ground state (in standby)
				remove_leds_from_cd(CD_SEARCH, search_leds);
				HAL_TIM_Base_Start_IT(&htim2);

				//re-activate breathing beacon and strobe lighting pattern.
				beacon_leds = (1 << 1) + (1 << 4);
				add_leds_to_cd(CD_BEACON, beacon_leds);
				set_domain_colour(CD_STROBE, RED);

				strobe_leds = 1 + (1 << 2) + (1 << 3) + (1 << 5);
				add_leds_to_cd(CD_STROBE, strobe_leds);
				set_domain_colour_and_brightness(CD_STROBE, ORANGE, 15);

				set_domain_colour_and_brightness(CD_SEARCH, WHITE, 10);
				this->drone_state = GROUND_STATE;
			} else if (input == 1) {										//to ground state (no standby)
				HAL_TIM_Base_Start_IT(&htim2);
				remove_leds_from_cd(CD_SEARCH, search_leds);
				beacon_leds = 1 + (1 << 2) + (1 << 3) + (1 << 5);
				add_leds_to_cd(CD_BEACON, beacon_leds);
				this->drone_state = GROUND_STATE;
			} else if (input == 2) {										//go to taxi_state
				remove_leds_from_cd(CD_SEARCH, search_leds);
				taxi_leds = (1 << 7) + (1 << 9);
				add_leds_to_cd(CD_TAXI, taxi_leds);
				this->drone_state = TAXI_STATE;
			} else if (input == 3) {										//go to take-off state
				remove_leds_from_cd(CD_SEARCH, search_leds);
				strobe_leds = (1 << 7) + (1 << 9);
				add_leds_to_cd(CD_STROBE, strobe_leds);
				this->drone_state = TAKE_OFF_STATE;
			} else if (input == 4) {										//go into flight state
				remove_leds_from_cd(CD_SEARCH, search_leds);
				strobe_leds = (1 << 7) + (1 << 9);
				add_leds_to_cd(CD_STROBE, strobe_leds);
				set_domain_colour_and_brightness(CD_STROBE, WHITE, 10);

				navigation_leds = (1 << 3) + (1 << 4) + (1 << 5);
				add_leds_to_cd(CD_NAVIGATION, navigation_leds);
				set_domain_colour_and_brightness(CD_NAVIGATION, GREEN, 15);
				this->drone_state = FLIGHT_STATE;
			} else if (input == 5) {										//go to landing state
				remove_leds_from_cd(CD_SEARCH, search_leds);

				strobe_leds = (1 << 7) + (1 << 9);
				add_leds_to_cd(CD_STROBE, strobe_leds);

				add_leds_to_cd(CD_LANDING, landing_leds);
				set_domain_colour_and_brightness(CD_LANDING, WHITE, 15);

				navigation_leds = 1 + (1 << 5);
				add_leds_to_cd(CD_NAVIGATION, navigation_leds);
				set_domain_colour_and_brightness(CD_NAVIGATION, GREEN, 15);
				this->drone_state = LANDING_STATE;
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

	static bool enter_standby = true;
	static uint8_t g_num_seconds = 0;

//	//Demonstration program that cycles through different drone states and the resulting lighting pattern.
	if (rev4.get_drone_state() == GROUND_STATE && g_num_seconds == 5) {
		if (enter_standby == false) {
			rev4.transition_to(2);	//go to TAXI_STATE
			rev4.activate_domain(CD_TAXI);
			enter_standby = true;
		} else {
			rev4.transition_to(1);	//stay in GROUND_STATE but show standby lighting pattern.
			enter_standby = false;
		}
		g_num_seconds = 0;
	} else if (rev4.get_drone_state() == TAXI_STATE && g_num_seconds == 1) {
		rev4.transition_to(1);	//go to TAKE_OFF_STATE
		g_num_seconds = 0;
	} else if(rev4.get_drone_state() == TAKE_OFF_STATE && g_num_seconds == 1) {
		rev4.transition_to(1);	//go to FLIGHT_STATE
		rev4.activate_domain(CD_NAVIGATION);
		g_num_seconds = 0;
	} else if (rev4.get_drone_state() == FLIGHT_STATE && g_num_seconds == 5) {
		rev4.transition_to(1);	//go to LANDING_STATE
		rev4.activate_domain(CD_LANDING);
		g_num_seconds = 0;
	} else if (rev4.get_drone_state() == LANDING_STATE && g_num_seconds == 5) {
		rev4.transition_to(0);	//go to SEARCH_STATE
		rev4.activate_domain(CD_BEACON);
		g_num_seconds = 0;
	} else if (rev4.get_drone_state() == SEARCH_STATE && g_num_seconds == 5) {
		rev4.transition_to(1);	//go to GROUND_STATE (no standby)
		rev4.activate_domain(CD_BEACON);
		g_num_seconds = 0;
	}

	g_num_seconds += 1;
}

void TIM7_100msCallback(TIM_HandleTypeDef *htim7) {
	static uint8_t stage = 0;
	static uint8_t led_index = 0;

	if (stage == 0) { 			// STROBE ON
		rev4.activate_domain(CD_STROBE);
		stage = 1;
	} else if (stage == 1) { 	// STROBE OFF
		rev4.deactivate_domain(CD_STROBE);
		stage = 2;
	} else if (stage == 2) {	// STROBE ON
		rev4.activate_domain(CD_STROBE);
		stage = 3;
	} else if (stage == 3) {	// STROBE OFF
		rev4.deactivate_domain(CD_STROBE);
		stage = 4;
	} else if (stage == 4) {	// OFF
		stage = 5;
	} else if (stage == 5) {	// OFF
		stage = 6;
	} else if (stage == 6) {	// BCN ON
		if (rev4.get_drone_state() >= FLIGHT_STATE) {
			rev4.activate_domain(CD_BEACON);
		}
		stage = 7;
	} else if (stage == 7) {	// BCN OFF
		if (rev4.get_drone_state() >= FLIGHT_STATE) {
			rev4.deactivate_domain(CD_BEACON);
		}
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

void TIM2_20msCallback(TIM_HandleTypeDef *htim2) {
	//Create a "breathing" pattern for when the drone first "turns on."
	static uint8_t brightness = 0;
	static uint8_t brightness_direction = 1;
	uint8_t brightness_max = 50;

	if (brightness <= 0) {
		brightness = 0;
		brightness_direction = 1;
	} else if (brightness >= brightness_max) {
		brightness = brightness_max;
		brightness_direction = -1;
	}

	rev4.set_domain_brightness(CD_BEACON, brightness);
	rev4.activate_domain(CD_BEACON);
	brightness += brightness_direction;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		TIM6_OneSecondCallback(htim);
	} else if (htim->Instance == TIM7) {
		TIM7_100msCallback(htim);
	} else if (htim->Instance == TIM2) {
		TIM2_20msCallback(htim);
	}
}
