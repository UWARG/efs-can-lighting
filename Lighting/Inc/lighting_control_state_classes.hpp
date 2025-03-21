/*
 * lighting_controller_states.hpp
 *
 *  Created on: Feb 26, 2025
 *      Author: folafatola
 */

#include "lighting_control_state_manager.hpp"


class LC_State_STARTUP: public LightingControlState {
public:
	LC_State_STARTUP();
	uint16_t *get_domain_leds() override;
	uint8_t get_allowed_domains() override;

private:
	uint16_t domain_leds[CD_LENGTH];					// Bitmask of LED's which are active in each domain
	uint8_t allowed_domains;
};

class LC_State_GROUND: public LightingControlState {
public:
	LC_State_GROUND();
	uint16_t *get_domain_leds() override;
	uint8_t get_allowed_domains() override;

private:
	uint16_t domain_leds[CD_LENGTH];
	uint8_t allowed_domains;
};

class LC_State_TAXI: public LightingControlState {
public:
	LC_State_TAXI();
	uint16_t *get_domain_leds();
	uint8_t get_allowed_domains();

private:
	uint16_t domain_leds[CD_LENGTH];
	uint8_t allowed_domains;
};


class LC_State_TAKEOFF: public LightingControlState {
public:
	LC_State_TAKEOFF();
	uint16_t *get_domain_leds();
	uint8_t get_allowed_domains();

private:
	uint16_t domain_leds[CD_LENGTH];
	uint8_t allowed_domains;
};

class LC_State_FLIGHT: public LightingControlState {
public:
	LC_State_FLIGHT();
	uint16_t *get_domain_leds();
	uint8_t get_allowed_domains();

private:
	uint16_t domain_leds[CD_LENGTH];
	uint8_t allowed_domains;
};


class LC_State_BRAKE: public LightingControlState {
public:
	LC_State_BRAKE();
	uint16_t *get_domain_leds();
	uint8_t get_allowed_domains();

private:
	uint16_t domain_leds[CD_LENGTH];
	uint8_t allowed_domains;
};


class LC_State_LANDING: public LightingControlState {
public:
	LC_State_LANDING();
	uint16_t *get_domain_leds();
	uint8_t get_allowed_domains();

private:
	uint16_t domain_leds[CD_LENGTH];
	uint8_t allowed_domains;
};


class LC_State_STANDBY : public LightingControlState {
public:
	LC_State_STANDBY();
	uint16_t *get_domain_leds();
	uint8_t get_allowed_domains();

private:
	uint16_t domain_leds[CD_LENGTH];
	uint8_t allowed_domains;
};


//If you reach this state, you might be cooked.
class LC_State_SEARCH: public LightingControlState {
public:
	LC_State_SEARCH();
	uint16_t *get_domain_leds();
	uint8_t get_allowed_domains();

private:
	uint16_t domain_leds[CD_LENGTH];
	uint8_t allowed_domains;
};

