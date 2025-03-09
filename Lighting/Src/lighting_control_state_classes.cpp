/*
 * lighting_controller_states.cpp
 *
 *  Created on: Feb 26, 2025
 *      Author: folafatola
 */

#include <lighting_control_state_classes.hpp>


LC_State_STARTUP::LC_State_STARTUP() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_MAIN] = 1 << LED_BOTTOM_NW | 1 << LED_BOTTOM_SW | 1 << LED_BOTTOM_SE | 1 << LED_BOTTOM_NE;
	allowed_domains = 1 << CD_MAIN;
}

uint16_t *LC_State_STARTUP::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_STARTUP::get_allowed_domains() {
	return allowed_domains;
}


LC_State_GROUND::LC_State_GROUND() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_BEACON] = 1 << LED_BOTTOM_NW | 1 << LED_BOTTOM_SW
			| 1 << LED_BOTTOM_SE | 1 << LED_BOTTOM_NE | 1 << LED_W_OUTER | 1 <<  LED_E_OUTER;
	allowed_domains = 1 << CD_BEACON;
}

uint16_t *LC_State_GROUND::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_GROUND::get_allowed_domains() {
	return allowed_domains;
}


LC_State_TAXI::LC_State_TAXI() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_TAXI] = 1 << LED_W_INNER | 1 << LED_E_INNER;
	domain_leds[CD_BEACON] = 1 << LED_W_OUTER | 1 << LED_E_OUTER;
	domain_leds[CD_BRAKE] = 1 << LED_BOTTOM_W | 1 << LED_BOTTOM_E;
	allowed_domains = 1 << CD_TAXI | 1 << CD_BEACON | 1 << CD_BRAKE;
}

uint16_t *LC_State_TAXI::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_TAXI::get_allowed_domains() {
	return allowed_domains;
}

LC_State_TAKEOFF::LC_State_TAKEOFF() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_BEACON] = 1 << LED_W_OUTER | 1 << LED_E_OUTER;
	domain_leds[CD_STROBE] = 1 << LED_W_INNER | 1 << LED_E_INNER;
	allowed_domains = 1 << CD_BEACON | 1 << CD_STROBE;
}

uint16_t *LC_State_TAKEOFF::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_TAKEOFF::get_allowed_domains() {
	return allowed_domains;
}


LC_State_FLIGHT::LC_State_FLIGHT() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_BEACON] = 1 << LED_W_OUTER | 1 << LED_E_OUTER;
	domain_leds[CD_STROBE] = 1 << LED_W_INNER | 1 << LED_E_INNER;
	domain_leds[CD_NAV] = 1 << LED_BOTTOM_NW | 1 << LED_BOTTOM_W | 1 << LED_BOTTOM_SW;
	allowed_domains = 1 << CD_BEACON | 1 << CD_STROBE | 1 << CD_NAV;
}

uint16_t *LC_State_FLIGHT::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_FLIGHT::get_allowed_domains() {
	return allowed_domains;
}


LC_State_BRAKE::LC_State_BRAKE() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_BRAKE] = 1 << LED_BOTTOM_W | 1 << LED_BOTTOM_E;
	allowed_domains = 1 << CD_BRAKE;
}

uint16_t *LC_State_BRAKE::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_BRAKE::get_allowed_domains() {
	return allowed_domains;
}


LC_State_LANDING::LC_State_LANDING() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_BEACON] = 1 << LED_W_OUTER | 1 << LED_E_OUTER;
	domain_leds[CD_STROBE] = 1 << LED_W_INNER | 1 << LED_E_INNER;
	domain_leds[CD_NAV] = 1 << LED_BOTTOM_NW | 1 << LED_BOTTOM_NE;
	domain_leds[CD_LANDING] = 1 << LED_BOTTOM_W | 1 << LED_BOTTOM_SW | 1 << LED_BOTTOM_SE | 1 << LED_BOTTOM_E;
	allowed_domains = 1 << CD_BEACON | 1 << CD_STROBE | 1 << CD_NAV | 1 << CD_LANDING;
}

uint16_t *LC_State_LANDING::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_LANDING::get_allowed_domains() {
	return allowed_domains;
}


LC_State_STANDBY::LC_State_STANDBY() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_STROBE] = 1 << LED_BOTTOM_NW | 1 << LED_BOTTOM_SW | 1 << LED_BOTTOM_SE | 1 << LED_BOTTOM_NE;
	domain_leds[CD_BEACON] = 1 << LED_BOTTOM_W | 1 << LED_BOTTOM_E;
	allowed_domains = 1 << CD_BEACON | 1 << CD_STROBE;
}

uint16_t *LC_State_STANDBY::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_STANDBY::get_allowed_domains() {
	return allowed_domains;
}


LC_State_SEARCH::LC_State_SEARCH() {
	for (int cd_idx = 0; cd_idx < CD_LENGTH; ++cd_idx) {
		domain_leds[cd_idx] = 0;
	}
	domain_leds[CD_SEARCH] = 1 << LED_BOTTOM_NW | 1 << LED_BOTTOM_E | 1 << LED_BOTTOM_SW;
	allowed_domains = 1 << CD_SEARCH;
}

uint16_t *LC_State_SEARCH::get_domain_leds() {
	return domain_leds;
}

uint8_t LC_State_SEARCH::get_allowed_domains() {
	return allowed_domains;
}
