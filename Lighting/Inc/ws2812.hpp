/*
 * ws2812.hpp
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni
 */

#ifndef INC_WS2812_HPP_
#define INC_WS2812_HPP_

#include <stdint.h>

static constexpr uint8_t PWM_LO = 19;
static constexpr uint8_t PWM_HI = 38;
static constexpr uint8_t BITS_PER_LED = 24;

// TODO: remove these
void initialize_bank_output_buffer_off(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad);
void initialize_bank_output_buffer_on(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad);
void initialize_bank_output_buffer_on(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad, uint8_t brightness);

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} RGB_colour_t;

typedef struct {
	uint16_t hue;		// TODO: limit this to 0-360 (degrees)
	uint8_t saturation;	// TODO: limit this to 0-100 (percentage)
	uint8_t lightness; 	// TODO: limit this to 0-100 (percentage)
} HSL_colour_t;

// WS2812 class definition.
// This is really just ONE led.
class WS2812 {
public:
	WS2812(uint8_t *output_buffer);
	void initialize_led_on();
//	void initialize_led_on(uint8_t *led_output_buffer);
	void initialize_led_off();
//	void initialize_led_off(uint8_t *led_output_buffer);
	void get_led_output_buffer(uint8_t *led_output_buffer);
	void set_led_colour(RGB_colour_t rgb_colour_value);			// TODO: proper error codes

private:
	uint8_t *buffer;	// Each LED keeps track of the start of it's buffer within a bank
	bool on;
};



#endif /* INC_WS2812_HPP_ */
