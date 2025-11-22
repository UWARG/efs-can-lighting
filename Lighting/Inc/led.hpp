#ifndef INC_LED_HPP
#define INC_LED_HPP

#include <stdint.h>
#include "conversions.hpp"


class LED {
    public:
        LED(
            uint8_t *colour_offsets,
            uint8_t *values_to_write,
            uint8_t num_channels
        );

        /**
	 * Initializes the LED to be full brightness on
	 *
	 * TODO: Add overloading to allow "on" to have a specified brightness
	 */
        void initialize_led_on();
        void initialize_led_on(uint8_t *led_output_buffer);

        /**
         * Initializes the LED to be full off
         *
         * Assumes that output buffer has already been set
         */
        void initialize_led_off();

        /**
         * Initializes the LED to be full off
         *
         * @param led_output_buffer : Pointer to the start of the output buffer for this LED
         */
        void initialize_led_off(uint8_t *led_output_buffer);

        
        /**
         * Sets the LED colour immediately.
         *
         * Since the LED itself works off of 8 bit R/G/B values, this is the function
         * that all HSL/HEX functions will call!
         *
         * @param rgb_colour_value : RGB_colour_t that you want set
         */
        virtual void set_led_colour(RGB_colour_t rgb_colour_value) = 0;	// TODO: proper error codes/return types

        /**
         * Sets the LED colour immediately based on the brightness
         *
         * @param rgb_colour_value : RGB_colour_t that you want set
         *
         * @param colour_brightness : RGB_colour_t brightness
         */
        void set_led_colour(RGB_colour_t rgb_colour_value,
                uint8_t colour_brightness);

        // TODO: set various methods for setting LED colour using custom HEX_COLOUR or HSL
        // Maybe take advantage of Unions, otherwise conversion.cpp is there for us!

        /**
         * Gets the current colour of the LED (in case it needs to be reset?)
         * 
         * TODO: return current LED colour
         */
        RGB_colour_t get_led_colour();

        /**
	 * Sets the LED colour brightness
	 *
	 * @param colour_brightness : RGB_colour_t brightness
	 */

	virtual void set_brightness(uint8_t colour_brightness) = 0;

    virtual uint8_t get_message_format_size() const = 0;

    protected:
        uint8_t NUM_CHANNELS;
        
        uint8_t *buffer;

        // Size of colour_offsets = Size of values_to_write
        uint8_t *COLOUR_OFFSETS;
        uint8_t *values_to_write;
        
        RGB_colour_t colour;
        
        uint8_t brightness;

        /**
	 * Writes colour data from this->colour to the buffer
	 *
	 * @param colour : RGB_colour_t "colour" to write to the buffer
	 *
	 */
	void push_colour_to_output_buffer();
    virtual void convert_colour_to_value() = 0;

    void set_buffer(uint8_t *output_buffer);
    void set_colour_offsets(uint8_t *colour_offsets);

};

#endif