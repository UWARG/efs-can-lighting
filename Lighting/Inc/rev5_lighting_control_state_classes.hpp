/*
 * lighting_controller_states.hpp
 *
 *  Created on: Feb 26, 2025
 *      Author: folafatola
 */

#include "lighting_control_state_manager.hpp"

typedef enum LedPosition_E {
	LED_SIDE_NW = 0,
	LED_OUTER_NW = 1,
	LED_INNER_NW = 2,
	LED_INNER_SW = 3,
	LED_OUTER_SW = 4,
	LED_SIDE_SW = 5,
	LED_SIDE_SE = 6,
	LED_OUTER_SE = 7,
	LED_INNER_SE = 8,
	LED_INNER_NE = 9,
	LED_OUTER_NE = 10,
	LED_SIDE_NE = 11,
} LedPosition_E;

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

