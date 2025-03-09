/*
 * lighting_demos.cpp
 *
 *  Created on: Mar 4, 2025
 *      Author: folafatola
 */

#include "lighting_demos.hpp"



// Initial setup call

void lighting_control_state_demo() {

	//Domain colours
	HAL_TIM_Base_Start_IT(&htim2);
	rev4.start_lighting_control(); //start lighting

	//allow all control domains.
	uint8_t all_domains_enabled = (1 << 7);
	rev4.configure_allowed_domains(all_domains_enabled);

	//set up the domain colours and brightness
	rev4.set_domain_colour_and_brightness(CD_MAIN, PURPLE, 15);
	rev4.set_domain_colour_and_brightness(CD_TAXI, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_LANDING, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_NAV, GREEN, 15);
	rev4.set_domain_colour_and_brightness(CD_BEACON, RED, 15); //CHANGE THIS TO RED.
	rev4.set_domain_colour_and_brightness(CD_STROBE, ORANGE, 15);
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
	LC_State_SEARCH search_state;

	LightingControlState *control_states[7];
	control_states[0] = &startup_state;
	control_states[1] = &ground_state;
	control_states[2] = &flight_state;
	control_states[3] = &brake_state;
	control_states[4] = &landing_state;
	control_states[5] = &standby_state;
	control_states[6] = &search_state;

	rev4.set_lighting_control_state(&startup_state);

	uint8_t brightness = 0;
	uint8_t brightness_direction = 1;
	uint8_t brightness_max = 50;

	int ctrl_idx = 0;
	int num_states = 7;
	int num_loops = 0;

	while (true) {

		if (num_loops == 150) {
			ctrl_idx = (ctrl_idx + 1) % num_states;
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
