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
 * @enum ControlDomain
 * Represents the different command mode that an LED might be responsive to.
 *
 * CD_MAIN - default mode where LED's respond to main.cpp commands
 * CD_STROBE - mode where LED's only accept CM_MAIN commands if NOT in a strobe pattern
 *
 * Add other modes as necessary
 */
enum ControlDomain {
	CD_MAIN = 0,
	CD_STROBE = 1,
	CD_LENGTH = 2	// increment this as necessary
};

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
	// TODO: add recolour_all_with_brightness


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

	// TODO: adds an LED to a control domain
	void add_led_to_cd(uint8_t index, ControlDomain domain);

	// TODO: removes an LED from a control domain
	void remove_led_from_cd(uint8_t index, ControlDomain domain);

	// Turns on a control domain !
	void enable_control_domain(ControlDomain domain);

	// Turns off a control domain !
	// Uncertain if we want to check if other domains are active and set LED's in those domains?
	void disable_control_domain(ControlDomain domain);

	void set_domain_colour(ControlDomain domain, RGB_colour_t colour);

	// TODO: add function to "recolour domain" (enables & sets colour)
	// Use this instead of set + enable?

private:
	static constexpr uint8_t NUM_LEDS = 6;
	uint8_t *dma_buffer;
	uint8_t *bank_buffer;
	WS2812 *leds;
	bool domain_state[CD_LENGTH];				// true when domain is active
	uint8_t domain_leds[CD_LENGTH];				// Bitmask of LED's which are active in each domain
	RGB_colour_t domain_colours[CD_LENGTH];		// Domain colour

	void initialize_bank_buffer_off();
	void initialize_bank_buffer_on();
	void initialize_dma_buffer();
};
#endif /* INC_LIGHTING_CONTROLLER_HPP_ */
