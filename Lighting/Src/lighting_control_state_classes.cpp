/*
 * lighting_controller_states.cpp
 *
 *  Created on: Feb 26, 2025
 *      Author: folafatola
 */

#include <lighting_control_state_classes.hpp>



LC_State_GROUND::LC_State_GROUND() {
	domain_leds_GROUND[CD_BEACON] |= 1 + (1 << 2) + (1 << 3) + (1 << 5) + (1 << 6) + (1 << 8);
}

uint8_t LC_State_GROUND::execute(uint16_t domain_leds_LC[]) {
	domain_leds_LC[CD_BEACON] = domain_leds_fround[CD_BEACON];
	domain_settings |= (1 << CD_BEACON);
	return domain_settings;
}

LC_State_TAXI::LC_State_TAXI() {
	domain_leds_taxi[CD_TAXI] |= (1 << 7) + (1 << 9);
	domain_leds_taxi[CD_BEACON] |= (1 << 6) + (1 << 8);
	domain_leds_taxi[CD_BRAKE] |= (1 << 1) + (1 << 4);
}

uint8_t LC_State_TAXI::execute(uint16_t domain_leds_LC[]) {
	domain_leds_LC[CD_TAXI] = domain_leds_taxi[CD_TAXI];
	domain_leds_LC[CD_BEACON] = domain_leds_taxi[CD_BEACON];
	domain_leds_LC[CD_BRAKE] = domain_leds_taxi[CD_BRAKE];
	domain_settings |= (1 << CD_TAXI) + (1 << CD_BEACON) + (1 << CD_BRAKE);
	return domain_settings;
}

LC_State_TAKEOFF::LC_State_TAKEOFF() {
	domain_leds_takeoff[CD_STROBE] |= (1 << 7) + (1 << 9);
	domain_leds_takeoff[CD_BEACON] |= (1 << 6) + (1 << 8);
}

uint8_t LC_State_TAKEOFF::execute(uint16_t domain_leds_LC[]) {
	domain_leds_LC[CD_STROBE] = domain_leds_takeoff[CD_STROBE];
	domain_leds_LC[CD_BEACON] = domain_leds_takeoff[CD_BEACON];
	domain_settings |= (1 << CD_STROBE) + (1 << CD_BEACON);
	return domain_settings;
}

LC_State_FLIGHT::LC_State_FLIGHT() {
	domain_leds_flight[CD_STROBE] |= (1 << 7) + (1 << 9);
	domain_leds_flight[CD_BEACON] |= (1 << 6) + (1 << 8);
	domain_leds_flight[CD_NAV] |= 1 + (1 << 1) + (1 << 2);
}

uint8_t LC_State_FLIGHT::execute(uint16_t domain_leds_LC[]) {
	domain_leds_LC[CD_STROBE] = domain_leds_flight[CD_STROBE];
	domain_leds_LC[CD_BEACON] = domain_leds_flight[CD_BEACON];
	domain_leds_LC[CD_NAV] = domain_leds_flight[CD_NAV];
	domain_settings |= (1 << CD_STROBE) + (1 << CD_BEACON) + (1 << CD_NAV);
	return domain_settings;
}

LC_State_BRAKE::LC_State_BRAKE() {
	domain_leds_brake[CD_BRAKE] |= (1 << 1) + (1 << 4);
}

uint8_t LC_State_BRAKE::execute(uint16_t domain_leds_LC[]) {
	domain_leds_LC[CD_BRAKE] = domain_leds_brake[CD_BRAKE];
	domain_settings |= (1 << CD_BRAKE);
}

LC_State_LANDING::LC_State_LANDING() {
	domain_leds_landing[CD_STROBE] |= (1 << 7) + (1 << 9);
	domain_leds_landing[CD_BEACON] |= (1 << 6) + (1 << 8);
	domain_leds_landing[CD_NAV] |= 1 + (1 << 5);
	domain_leds_landing[CD_LANDING] |= (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4);
}

uint8_t LC_State_LANDING::execute(uint16_t domain_leds_LC[]) {
	domain_leds_LC[CD_STROBE] = domain_leds_landing[CD_STROBE];
	domain_leds_LC[CD_BEACON] = domain_leds_landing[CD_BEACON];
	domain_leds_LC[CD_NAV] = domain_leds_landing[CD_NAV];
	domain_leds_LC[CD_LANDING] = domain_leds_landing[CD_LANDING];
	domain_settings |= (1 << CD_STROBE) + (1 << CD_BEACON) + (1 << CD_NAV) + (1 << CD_LANDING);
	return domain_settings;
}

LC_State_STANDBY::LC_State_STANDBY() {
	domain_leds_standby[CD_BEACON] |= (1 << 1) + (1 << 4);
	domain_leds_standby[CD_STROBE] |= 1 + (1 << 2) + (1 << 3) + (1 << 5);
}

uint8_t LC_State_STANDBY::execute(uint16_t domain_leds_LC[]) {
	domain_leds_LC[CD_STROBE] = domain_leds_standby[CD_STROBE];
	domain_leds_LC[CD_BEACON] = domain_leds_standby[CD_BEACON];
	domain_settings |= (1 << CD_STROBE) + (1 << CD_BEACON);
	return domain_settings;
}

LC_State_SEARCH::LC_State_SEARCH() {
	for (int i = 0; i < 6; ++i) {
		domain_leds_search[CD_SEARCH] |= (1 << i);
	}
}
