
#ifndef INC_SK6812_HPP_
#define INC_SK6812_HPP_

#include <stdint.h>

#include "conversions.hpp"
#include "led.hpp"



/**
 * @class SK6812
 * @brief A class representing a single SK6812 LED
 *
 * This class manages the state of a single SK6812 LED by directly manipulating
 * the memory buffer associated with it's output data. When you set an LED,
 * it will store state (RGB), and immediately set LED colour.
 */
class SK6812 : public LED {
public:
	
	/**
	 * Default Constructor
	 *
	 * You must then call `initialize_<>()` with a pointer to the output buffer
	 */
	SK6812();
	/**
	 * Constructs a SK6812 object
	 *
	 * @param output_buffer : Pointer to the start of the output buffer
	 */
	SK6812(uint8_t *output_buffer);

	static constexpr uint8_t PWM_LO = 19;
	static constexpr uint8_t PWM_HI = 38;

	static constexpr uint8_t MESSAGE_FORMAT_SIZE = 32;
	static constexpr uint8_t BITS_PER_COLOUR = 8;

	uint8_t get_message_format_size() const override {
        return MESSAGE_FORMAT_SIZE;
    }

	/**
	 * Sets the LED colour immediately.
	 *
	 * Since the LED itself works off of 8 bit R/G/B values, this is the function
	 * that all HSL/HEX functions will call!
	 *
	 * @param rgb_colour_value : RGB_colour_t that you want set
	 */
	void set_led_colour(RGB_colour_t rgb_colour_value) override;	// TODO: proper error codes/return types

	// TODO: set various methods for setting LED colour using custom HEX_COLOUR or HSL
	// Maybe take advantage of Unions, otherwise conversion.cpp is there for us!


	/**
	 * Sets the LED colour brightness
	 *
	 * @param colour_brightness : RGB_colour_t brightness
	 */

	void set_brightness(uint8_t colour_brightness) override;

private:
    enum ColourIndex {
        GREEN = 0,
        RED = 1,
        BLUE = 2,
        WHITE = 3
    };
	void convert_colour_to_value() override;

	static constexpr uint8_t NUM_CHANNELS = 4;
	uint8_t colour_offsets[NUM_CHANNELS];
	uint8_t values_to_write[NUM_CHANNELS*BITS_PER_COLOUR];
};

#endif /* INC_SK6812_HPP_ */
