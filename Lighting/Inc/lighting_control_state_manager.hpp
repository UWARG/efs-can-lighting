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
	virtual uint8_t execute(uint16_t domain_leds[]) = 0;
};

#endif /* INC_LIGHTING_CONTROL_STATE_MANAGER_HPP_ */
