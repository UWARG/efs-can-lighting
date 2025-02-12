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
 * CD_MAIN - default mode for LED's
 * CD_BEACON - beacon lights (red pulse 1x per second), indicates LV ON
 * CD_STROBE - strobe lights (Double white flash 1x per second), indicates HV ON
 *
 *
 * Add other modes as necessary
 */
enum ControlDomain {
	CD_MAIN = 0,
	CD_STANDBY = 1,
	CD_BEACON = 2,
	CD_STROBE = 3,
	CD_LANDING_TAKEOFF = 4,
	CD_NAVIGATION = 5,
	CD_BRAKE = 6,
	CD_SEARCH = 7,
	CD_LENGTH = 8 // increment this as necessary
};

/**
 * @enum State
 * GROUND_STATE
 * TAXI_STATE
 * FLIGHT_STATE
 * SEARCH_STATE
 * TAKEOFF_STATE
 * LANDING_STATE
 *
 */

typedef enum State {
	GROUND_STATE = 0,
	TAXI_STATE = 1,
	FLIGHT_STATE = 2,
	SEARCH_STATE = 3,
	TAKEOFF_STATE = 4,
	LANDING_STATE = 5,
	NUM_STATES = 6	//increment this as necessary.
} State;

// TODO: Make these public
void run_lighting_board();

class LightingController {
public:
	/**
	 * TODO: Initialize lighting controller with a reference to the led bank output
	 */
	LightingController(uint8_t *dma_output_buffer, uint8_t *bank_output_buffer,
			WS2812 *leds, TIM_HandleTypeDef *timer, uint16_t timer_channel);

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
	void recolour_by_index(uint8_t index, RGB_colour_t desired_colour,
			uint8_t brightness);

	/**
	 * Adds an LED to a Control Domain
	 *
	 * @param index : LED index on the board
	 * @param domain : Control Domain to add the LED to.
	 */
	void add_led_to_cd(uint8_t index, ControlDomain domain);

	/**
	 * Removes an LED from a Control Domain
	 *
	 * @param index : LED index on the board
	 * @param domain : Control Domain to remove the LED from
	 */
	void remove_led_from_cd(uint8_t index, ControlDomain domain);

	/**
	 * Enable (turn on) a Control Domain
	 *
	 * This will assign all LED's that are not superceded by this domain the brightness
	 * and colour of the specified domain. It is necesasry to call `set_domain_colour` in
	 * order to update the colour of the domain ATM.
	 *
	 * @param domain : domain to be enabled.
	 */
	void activate_domain(ControlDomain domain);

	/**
	 * Disables (turn off) a Control Domain
	 *
	 * Any LED that is part of another control domain will revert to that control domains
	 * colour & brightness setting.
	 *
	 * @param domain : domain to be disabled
	 */
	void deactivate_domain(ControlDomain domain);

	void allow_domain(ControlDomain domain);

	void disallow_domain(ControlDomain domain);

	/**
	 * Sets the colour & brightness of a Control Domain
	 *
	 * TODO: future functions for only brightness/colour.
	 *
	 * @param domain : domain to be modified
	 * @param colour : desired colour
	 * @param brightness : desired brightness
	 */
	void set_domain_colour_and_brightness(ControlDomain domain, RGB_colour_t colour,
			uint8_t brightness);

	/**
	 * This will set just the brightness of all the LEDs that are part of a control domain.
	 *
	 * @param domain : domain to be modified
	 * @param colour : desired colour
	*/
	void set_domain_colour(ControlDomain domain, RGB_colour_t colour);

	/**
	 * This will set just the colour of all the LEDs that are part of a control domain.
	 *
	 * @param domain : domain to be modified
	 * @param brightness : desired brightness
	*/
	void set_domain_brightness(ControlDomain domain, uint8_t brightness);


	/**
	 * This is a mealy state machine that transitions from one drone state to another.
	 * The "output" is the highest priority active control domain.
	 *
	 * @param input : input that determines the next drone state and output.
	 */

	void transition_to(uint8_t input);

	void transition_to_search_state();

	/*
	 * Returns the state of the drone
	 */

	State get_drone_state();




	// TODO: add function to "recolour domain" (enables & sets colour)
	// Use this instead of set + enable?

private:
	static constexpr uint8_t NUM_LEDS = 10;
	State drone_state; //state of the drone.

	uint8_t *dma_buffer;
	uint8_t *bank_buffer;
	WS2812 *leds;

	uint8_t domain_allowed;					// true when a domain is allowed to be active
	uint8_t domain_active;					// true when domain is active
	uint16_t domain_leds[CD_LENGTH];			// Bitmask of LED's which are active in each domain
	RGB_colour_t domain_colours[CD_LENGTH];	// Domain colour
	uint8_t domain_brightness[CD_LENGTH];	// Domain brightness

	TIM_HandleTypeDef *htimx;				//timer handle
	uint16_t tim_channel_x;					//timer channel

	void initialize_bank_buffer_off();
	void initialize_bank_buffer_on();
	void initialize_dma_buffer();
};
#endif /* INC_LIGHTING_CONTROLLER_HPP_ */
