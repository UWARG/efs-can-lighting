/*
 * lighting_demos.cpp
 *
 *  Created on: Mar 4, 2025
 *      Author: folafatola
 */

#include "lighting_demos.hpp"

void lighting_control_domain_demo() {

	HAL_TIM_Base_Start_IT(&htim2);
	rev4.start_lighting_control(); //start lighting

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
	uint8_t brightness = 0;

	uint8_t BRIGHTNESS_MAX = 100;

	rev4.allow_domain(CD_MAIN);

	LC_State_STARTUP startup_state;
	rev4.set_lighting_control_state(&startup_state);

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

		rev4.set_domain_colour_and_brightness(CD_MAIN, my_colour, brightness);
		rev4.activate_domain(CD_MAIN);
	}
}


void lighting_control_state_demo() {

	//Domain colours
	HAL_TIM_Base_Start_IT(&htim2);
	rev4.start_lighting_control(); //start lighting

	//allow all control domains.
	uint8_t all_domains_enabled = (1 << 7);
	rev4.configure_allowed_domains(all_domains_enabled);

	//set up the domain colours and brightness
	rev4.set_domain_colour_and_brightness(CD_MAIN, PURPLE, 5);
	rev4.set_domain_colour_and_brightness(CD_TAXI, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_LANDING, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_NAV, BLUE, 15);
	rev4.set_domain_colour_and_brightness(CD_BEACON, RED, 15); //CHANGE THIS TO RED.
	rev4.set_domain_colour_and_brightness(CD_STROBE, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_BRAKE, ORANGE, 15);
	rev4.set_domain_colour_and_brightness(CD_SEARCH, WHITE, 15);

	rev4.configure_active_domains(255);

	//Declare control states
	LC_State_STARTUP startup_state;
	LC_State_GROUND ground_state;
	LC_State_FLIGHT flight_state;
	LC_State_BRAKE brake_state;
	LC_State_LANDING landing_state;
	LC_State_STANDBY standby_state;
	LC_State_TAKEOFF takeoff_state;
	LC_State_SEARCH search_state;
	LC_State_TAXI taxi_state;

	LightingControlState *control_states[8];
	control_states[0] = &startup_state;
	control_states[1] = &ground_state;
	control_states[2] = &taxi_state;
	control_states[3] = &takeoff_state;
	control_states[4] = &flight_state;
	control_states[5] = &landing_state;

	rev4.set_lighting_control_state(&startup_state);

	uint8_t brightness = 0;
	uint8_t brightness_direction = 1;
	uint8_t brightness_max = 50;

	int ctrl_idx = 0;
	int num_states = 8;
	int num_loops = 0;

	while (true) {
		if (num_loops == 150) {
			ctrl_idx = (ctrl_idx + 1) % num_states;
			if (ctrl_idx == 0) {
				rev4.set_domain_colour(CD_BEACON, WHITE);
			} else if (ctrl_idx == 1) {
				rev4.set_domain_colour(CD_NAV, BLUE);
				rev4.set_domain_colour(CD_BEACON, RED);
			} else if (ctrl_idx == 2) {
				rev4.set_domain_colour(CD_NAV, GREEN);
				rev4.set_domain_colour(CD_BEACON, WHITE);
			} else if (ctrl_idx == 3) {
				rev4.set_domain_colour(CD_NAV, GREEN);
				rev4.set_domain_colour(CD_BEACON, WHITE);
			} else if (ctrl_idx == 4) {
				rev4.set_domain_colour(CD_NAV, BLUE);
				rev4.set_domain_colour(CD_BEACON, RED);
			} else if (ctrl_idx == 5) {
				rev4.set_domain_colour(CD_NAV, BLUE);
				rev4.set_domain_colour(CD_BEACON, WHITE);
			}

			rev4.set_lighting_control_state(control_states[ctrl_idx]);
			num_loops = 0;
		}

		//Create a "breathing" pattern for when the drone is on the ground.
		if (ctrl_idx == 1) {	//if we are in the GROUND state, then breathe.
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
		HAL_Delay(20);
		num_loops += 1;
	}
}
