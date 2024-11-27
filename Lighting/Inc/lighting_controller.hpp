/*
 * lighting_controller.hpp
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni
 */

#ifndef INC_LIGHTING_CONTROLLER_HPP_
#define INC_LIGHTING_CONTROLLER_HPP_

#include <cstdint>

#include "conversions.hpp"
#include "ws2812.hpp"

/**
 * @enum CommandMode
 * Represents the different command mode that an LED might be responsive to.
 *
 * CM_MAIN - default mode where LED's respond to main.cpp commands
 * CM_STROBE - mode where LED's only accept CM_MAIN commands if NOT in a strobe pattern
 *
 * Add other modes as necessary
 */

// TODO: Make these public
void run_lighting_board();

class LightingController {
public:
	/**
	 * TODO: Initialize lighting controller with a reference to the led bank output
	 */
	LightingController(uint8_t *dma_output_buffer, uint8_t *bank_output_buffer,
			WS2812 *leds);

	void start_lighting_control();

	/**
	 * Re-Colours all of the LED's with a desired colour
	 *
	 * @param desired_colour : desired colour
	 */
	void recolour_all(RGB_colour_t desired_colour);
	// TODO: add recolour_all_with_brightness

	/**
	 * Re-Colours only the specific LED
	 *
	 * @param index : index of the LED (starts at 0 - see board designators)
	 * @param desired_colour : desired colour
	 */
	void recolour_by_index(uint8_t index, RGB_colour_t desired_colour);

private:
	uint8_t *dma_buffer;
	uint8_t *bank_buffer;
	WS2812 *leds;

	void initialize_bank_buffer_off();
	void initialize_bank_buffer_on();
	void initialize_dma_buffer();
};
#endif /* INC_LIGHTING_CONTROLLER_HPP_ */
