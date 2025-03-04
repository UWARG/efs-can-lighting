/*
 * lighting_control_state_manager.hpp
 *
 *  Created on: Feb 26, 2025
 *      Author: folafatola
 */

#ifndef INC_LIGHTING_CONTROL_STATE_MANAGER_HPP_
#define INC_LIGHTING_CONTROL_STATE_MANAGER_HPP_

#include "conversions.hpp"


class LightingControlState {
public:
	virtual void execute() = 0;
	virtual uint16_t *get_domain_leds() = 0;
	virtual uint8_t get_allowed_domains() = 0;
};

#endif /* INC_LIGHTING_CONTROL_STATE_MANAGER_HPP_ */
