/*
 * lighting_controller_states.hpp
 *
 *  Created on: Feb 26, 2025
 *      Author: folafatola
 */

#include "lighting_control_state_manager.hpp"

class LC_State_GROUND: public LightingControlState {
public:
	LC_State_GROUND();
	uint8_t execute(uint16_t domain_leds_LC[]) override;

private:
	uint16_t domain_leds_ground[CD_LENGTH];
	uint8_t domain_settings;
};


class LC_State_TAXI: public LightingControlState {
public:
	LC_State_TAXI();
	uint8_t execute(uint16_t domain_leds_LC[]) override;

private:
	uint16_t domain_leds_taxi[CD_LENGTH];
	uint8_t domain_settings;
};


class LC_State_TAKEOFF: public LightingControlState {
public:
	LC_State_TAKEOFF();
	uint8_t execute(uint16_t domain_leds_LC[]) override;

private:
	uint16_t domain_leds_takeoff[CD_LENGTH];
	uint8_t domain_settings;
};

class LC_State_FLIGHT: public LightingControlState {
public:
	LC_State_FLIGHT();
	uint8_t execute(uint16_t domain_leds_LC[]) override;

private:
	uint16_t domain_leds_flight[CD_LENGTH];
	uint8_t domain_settings;
};


class LC_State_BRAKE: public LightingControlState {
public:
	LC_State_BRAKE();
	uint8_t execute(uint16_t domain_leds_LC[]) override;

private:
	uint16_t domain_leds_brake[CD_LENGTH];
	uint8_t domain_settings;
};


class LC_State_LANDING: public LightingControlState {
public:
	LC_State_LANDING();
	uint8_t execute(uint16_t domain_leds_LC[]) override;

private:
	uint16_t domain_leds_landing[CD_LENGTH];
	uint8_t domain_settings;
};

class LC_State_STANDBY : public LightingControlState {
public:
	LC_State_STANDBY();
	uint8_t execute(uint16_t domain_leds_LC[]);
private:
	uint16_t domain_leds_standby[CD_LENGTH];
	uint8_t domain_settings;
};

//If you reach this state, you might be cooked.
class LC_State_SEARCH: public LightingControlState {
public:
	LC_State_SEARCH();
	uint8_t execute(uint16_t domain_leds_LC[]);
private:
	uint16_t domain_leds_search[CD_LENGTH];
	uint8_t domain_settings;
};

