/*
 * lighting_controller_states.cpp
 *
 *  Created on: Feb 26, 2025
 *      Author: folafatola
 */

#include <lighting_control_state_classes.hpp>


void LC_State_GROUND::execute() {
	domain_leds[CD_BEACON] = 1 + (1 << 2) + (1 << 3) + (1 << 5) + (1 << 6) + (1 << 8);
	allowed_domains = (1 << CD_BEACON);
}

uint16_t *LC_State_GROUND::get_domain_leds() {
	return domain_leds;
}

volatile uint8_t LC_State_GROUND::get_allowed_domains() {
	return allowed_domains;
}



void LC_State_TAXI::execute() {
	domain_leds[CD_TAXI] = (1 << 7) + (1 << 9);
	domain_leds[CD_BEACON] = (1 << 6) + (1 << 8);
	domain_leds[CD_BRAKE] = (1 << 1) + (1 << 4);
	allowed_domains = (1 << CD_TAXI) + (1 << CD_BEACON) + (1 << CD_BRAKE);
}

uint16_t *LC_State_TAXI::get_domain_leds() {
	return domain_leds;
}

volatile uint8_t LC_State_TAXI::get_allowed_domains() {
	return allowed_domains;
}



void LC_State_TAKEOFF::execute() {
	domain_leds[CD_BEACON] = (1 << 6) + (1 << 8);
	domain_leds[CD_STROBE] = (1 << 7) + (1 << 9);
	allowed_domains = (1 << CD_BEACON) + (1 << CD_STROBE);
}

uint16_t *LC_State_TAKEOFF::get_domain_leds() {
	return domain_leds;
}

volatile uint8_t LC_State_TAKEOFF::get_allowed_domains() {
	return allowed_domains;
}



void LC_State_FLIGHT::execute() {
	domain_leds[CD_BEACON] = (1 << 6) + (1 << 8);
	domain_leds[CD_STROBE] = (1 << 7) + (1 << 9);
	domain_leds[CD_NAV] = 1 + (1 << 1) + (1 << 2);
	allowed_domains = (1 << CD_BEACON) + (1 << CD_STROBE) + (1 << CD_NAV);
}

uint16_t *LC_State_FLIGHT::get_domain_leds() {
	return domain_leds;
}

volatile uint8_t LC_State_FLIGHT::get_allowed_domains() {
	return allowed_domains;
}



void LC_State_BRAKE::execute() {
	domain_leds[CD_BRAKE] = (1 << 1) + (1 << 4);
	allowed_domains = (1 << CD_BRAKE);
}

uint16_t *LC_State_BRAKE::get_domain_leds() {
	return domain_leds;
}

volatile uint8_t LC_State_BRAKE::get_allowed_domains() {
	return allowed_domains;
}



void LC_State_LANDING::execute() {
	domain_leds[CD_BEACON] = (1 << 6) + (1 << 8);
	domain_leds[CD_STROBE] = (1 << 7) + (1 << 9);
	domain_leds[CD_NAV] = 1 + (1 << 5);
	domain_leds[CD_LANDING] = (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4);
	allowed_domains = (1 << CD_BEACON) + (1 << CD_STROBE) + (1 << CD_NAV) + (1 << CD_LANDING);
}

uint16_t *LC_State_LANDING::get_domain_leds() {
	return domain_leds;
}

volatile uint8_t LC_State_LANDING::get_allowed_domains() {
	return allowed_domains;
}



void LC_State_STANDBY::execute() {
	domain_leds[CD_BEACON] = (1 << 1) + (1 << 4);
	domain_leds[CD_STROBE] = 1 + (1 << 2) + (1 << 3) + (1 << 5);
	allowed_domains = (1 << CD_TAXI) + (1 << CD_BEACON) + (1 << CD_BRAKE);
}

uint16_t *LC_State_STANDBY::get_domain_leds() {
	return domain_leds;
}

volatile uint8_t LC_State_STANDBY::get_allowed_domains() {
	return allowed_domains;
}

