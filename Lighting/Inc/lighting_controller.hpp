/*
 * lighting_controller.hpp
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni and Fola
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


	/**
	 * Start sending lighting control data to neopixels.
	 * Send values corresponding to neopixel brightness to capture compare register (CCR).
	 */

	void start_lighting_control();


	/**
	 * Sets all LEDs under controller's domain to desired colors.
	 *
	 * @param desired_colour : RGB_colour_t that you want set
	 */
	void recolour_all(RGB_colour_t desired_colour);


	/**
	 * Sets all LEDs under controller's domain to desired colors.
	 *
	 * @param desired_colour : RGB_colour_t that you want set
	 *
	 * @param brightness : sets the same brightness for each LEDs
	 */
	void recolour_all(RGB_colour_t desired_colour, uint8_t brightness);



	/**
	 * Sets all LEDs under controller's domain to desired colors.
	 *
	 * @param index : The LED on the lighting board that you want to send.
	 *
	 * @param desired_colour: RGB_colour_t that you want set.
	 */
	void recolour_by_index(uint8_t index, RGB_colour_t desired_colour);



	/**
	 * Sets all LEDs under controller's domain to desired colors.
	 *
	 * @param index : The LED on the lighting board that you want to send.
	 *
	 * @param desired_colour: RGB_colour_t that you want set.
	 *
	 * @param brightness: the brightness of a specific LED on the LED board.
	 */
	void recolour_by_index(uint8_t index, RGB_colour_t desired_colour, uint8_t brightness);

private:
	uint8_t *dma_buffer;	//pointer to dma buffer
	uint8_t *bank_buffer;	//led output bank buffer
	WS2812 *leds;			//pointer to an array of LEDS

	void initialize_bank_buffer_off();
	void initialize_bank_buffer_on();
	void initialize_dma_buffer();
};
#endif /* INC_LIGHTING_CONTROLLER_HPP_ */
