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
#include "lighting_controller.hpp"

//#define STARTUP_SEQUENCE_1 // very basic selftest

extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim;

// TODO: custom types?
static constexpr uint8_t NUM_LEDS = 10;
static constexpr uint8_t NUM_LEDS_PADDING = 10;
static constexpr uint16_t DMA_OUTPUT_BUFFER_SIZE = (NUM_LEDS
		+ NUM_LEDS_PADDING * 2) * 24 * 2;		// TODO: remove magic num
static constexpr uint16_t BANK_OUTPUT_BUFFER_SIZE = (NUM_LEDS
		+ NUM_LEDS_PADDING * 2) * 24 * 2;	// TODO: remove magic num

uint8_t dma_output_buffer[DMA_OUTPUT_BUFFER_SIZE];
uint8_t bank_output_buffer[BANK_OUTPUT_BUFFER_SIZE];

WS2812 leds[NUM_LEDS]; // TODO: make this work

// Initial setup call
LightingController rev4(dma_output_buffer, bank_output_buffer, leds, &htim2, TIM_CHANNEL_1); // TODO: Once we have custom functions registered as callbacks.....

// Temporary (ish) function with exemplar code that allows us to test lighting board functionality without needing CAN commands
void run_lighting_board() {

	// Call to start lighting control
	rev4.start_lighting_control();
	HAL_TIM_Base_Start_IT(&htim2);

	// DOMAIN SETUP
	// TODO: move Control Domain building to special functions

	// allow all of our domains
	// comment any of these out to see the effect of allowing command domains
	uint8_t all_domains_enabled = (1 << 7);
	rev4.configure_allowed_domains(all_domains_enabled);

	//set up the domain colours and brightness
	rev4.set_domain_colour_and_brightness(CD_MAIN, PURPLE, 15);
	rev4.set_domain_colour_and_brightness(CD_TAXI, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_LANDING, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_NAV, GREEN, 15);
	rev4.set_domain_colour_and_brightness(CD_BEACON, RED, 15); //CHANGE THIS TO RED.
	rev4.set_domain_colour_and_brightness(CD_STROBE, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_BRAKE, ORANGE, 15);
	rev4.set_domain_colour_and_brightness(CD_SEARCH, WHITE, 15);

	uint8_t brightness = 0;
	uint8_t brightness_direction = 1;
	uint8_t brightness_max = 50;

	//Instantiate lighting state objects and pass them into the set_lighting_control_state function to enter a lighting state.
	LC_State_LANDING landing_state;
	LC_State_GROUND ground_state;
	LC_State_FLIGHT flight_state;
	LC_State_STARTUP startup_state;

	rev4.set_lighting_control_state(&ground_state);

	int num_loops = 0;

	while (true) {
		//Create a "breathing" pattern for when the drone first "turns on."
		if (rev4.get_lighting_control_state() == &ground_state) {
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
			HAL_Delay(20);
		}

		if (num_loops == 50 && rev4.get_lighting_control_state() == &ground_state) {
			num_loops = 0;
			rev4.set_lighting_control_state(&landing_state);
		}
		num_loops += 1;
	}
}

LightingController::LightingController(uint8_t *dma_output_buffer,
		uint8_t *bank_output_buffer, WS2812 *leds, TIM_HandleTypeDef *timer, uint16_t timer_channel) {
	this->dma_buffer = dma_output_buffer;
	this->bank_buffer = bank_output_buffer;
	this->leds = leds;
	this->lighting_controller_tim_handle = timer;
	this->lighting_controller_tim_channel = timer_channel;
	this->lighting_control_state = nullptr;
	initialize_bank_buffer_on();
	initialize_dma_buffer();
	// Initialize all of the internal LED's as well
	for (int i = 0; i < NUM_LEDS; ++i) {
		this->leds[i].initialize_led_off(
				bank_output_buffer + NUM_LEDS_PADDING * 24 + 24 * i);
	}
}

void LightingController::start_lighting_control() {
	HAL_TIM_PWM_Start_DMA(this->lighting_controller_tim_handle, this->lighting_controller_tim_channel,
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
//void LightingController::add_led_to_cd(uint8_t index, ControlDomain domain) {
//	// set the index in the domain
//	this->domain_leds[domain] |= 1 << index;
//}
//
//void LightingController::remove_led_from_cd(uint8_t index,
//		ControlDomain domain) {
//	// clear the index from the domain
//	this->domain_leds[domain] &= ~(1 << index);
//}

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
	if (this->lighting_control_state == nullptr) {
		return;
	}
	uint16_t *domain_leds = this->lighting_control_state->get_domain_leds();
	if (this->domain_allowed & (1 << domain)) {
		this->domain_active |= 1 << domain;	// TODO: have active/disabled booleans
		for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
			if (this->domain_active & (1 << cd_idx)) { // IF THIS DOMAIN IS ACTIVE
				for (int j = 0; j < NUM_LEDS; ++j) {
					if (domain_leds[cd_idx] & (1 << j)) {
						this->leds[j].set_led_colour(domain_colours[cd_idx],
								domain_brightness[cd_idx]);
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
	if (this->lighting_control_state == nullptr) {
		return;
	}
	uint16_t *domain_leds = this->lighting_control_state->get_domain_leds();
	this->domain_active &= ~(1 << domain);
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		if (this->domain_active & (1 << cd_idx)) { // IF THIS DOMAIN IS ACTIVE
			for (int j = 0; j < NUM_LEDS; ++j) {
				if (domain_leds[cd_idx] & (1 << j)) {
					this->leds[j].set_led_colour(domain_colours[cd_idx],
							domain_brightness[cd_idx]);
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
	this->domain_active &= ~(1 << domain);
}

void LightingController::configure_allowed_domains(uint8_t allowed_domains) {
	for (int i = 0; i < CD_LENGTH; ++i) {
		if (allowed_domains & (1 << i)) {	//if this domain should be allowed
			this->domain_allowed |= (1 << i);
		} else {
			this->domain_allowed &= ~(1 << i);
		}
	}
}

void LightingController::configure_active_domains(uint8_t active_domains) {
	if (this->lighting_control_state == nullptr) {
		return;
	}
	uint16_t *domain_leds = this->lighting_control_state->get_domain_leds();
	for (int i = 0; i <= CD_SEARCH; ++i) {
		ControlDomain domain = static_cast<ControlDomain>(i);
		if (active_domains & (1 << i)) {
			activate_domain(domain);
		}
	}
}

////////////////////
// STATE MACHINE FN
////////////////////
void LightingController::exit_current_state() {
	if (lighting_control_state != nullptr) { 						//clear state
		uint16_t *domain_leds = this->lighting_control_state->get_domain_leds();
		for (int i = 0; i < CD_LENGTH; ++i) {
			if (this->domain_active & (1 << i)) { // If the domain was previously active.
				for (int j = 0; j < NUM_LEDS; ++j) {
					if (domain_leds[i] & (1 << j)) {
						this->leds[j].set_led_colour(domain_colours[i], 0); //"Disable" leds that we don't want during next state.
					}
				}
			}
		}
	}
}

void LightingController::set_lighting_control_state(LightingControlState *state) {
	exit_current_state();
	lighting_control_state = state;
}


void LightingController::execute_state() {
	if (this->get_lighting_control_state() != nullptr) { //If a state has been set.
		this->domain_allowed = this->lighting_control_state->get_allowed_domains();
	}
}

LightingControlState *LightingController::get_lighting_control_state() {
	return lighting_control_state;
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
	static uint8_t led_index = 0;
	HAL_TIM_Base_Start_IT(&htim2);

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
		rev4.activate_domain(CD_BEACON);
		stage = 7;
	} else if (stage == 7) {	// BCN OFF
		rev4.deactivate_domain(CD_BEACON);
		stage = 0;
		HAL_TIM_Base_Stop_IT(htim7);
	}

	if (rev4.get_lighting_control_state() != nullptr) { //If a state has been SET.
		uint8_t allowed_domains = rev4.get_lighting_control_state()->get_allowed_domains();
		rev4.configure_active_domains(allowed_domains);
	}
}

void TIM2_10msCallback(TIM_HandleTypeDef *htim2) {
	static uint8_t state_executions_per_100ms = 0;
	rev4.execute_state();
	state_executions_per_100ms += 1;
	if (state_executions_per_100ms == 8) {
		state_executions_per_100ms = 0;
		HAL_TIM_Base_Stop_IT(htim2);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		TIM6_OneSecondCallback(htim);
	} else if (htim->Instance == TIM7) {
		TIM7_100msCallback(htim);
	} else if (htim->Instance == TIM2) {
		TIM2_10msCallback(htim);
	}
}
