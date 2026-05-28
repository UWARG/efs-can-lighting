#include "led.hpp"

LED::LED(uint8_t *colour_offsets, uint8_t *values_to_write, uint8_t num_channels) :
    COLOUR_OFFSETS(colour_offsets),
    values_to_write(values_to_write),
    NUM_CHANNELS(num_channels)
{}

void LED::initialize_led_on() {
    set_led_colour(WHITE, 100);
}

void LED::initialize_led_on(uint8_t *led_output_buffer) {
	this->buffer = led_output_buffer;

    set_led_colour(WHITE, 100);
}

void LED::initialize_led_off(uint8_t *led_output_buffer) {
	this->buffer = led_output_buffer;

    set_led_colour(WHITE, 0);
}

void LED::initialize_led_off() {
    set_led_colour(WHITE, 0);
}


void LED::push_colour_to_output_buffer() {
    for (int i = 0; i < get_message_format_size(); ++i) {
        buffer[i] = values_to_write[i];
    }
}

RGB_colour_t LED::get_led_colour() {
	return this->colour;
}

void LED::set_led_colour(RGB_colour_t rgb_colour_value, uint8_t colour_brightness) {
    set_led_colour(rgb_colour_value);
    set_brightness(colour_brightness);
}

void LED::set_buffer(uint8_t *output_buffer) {
    buffer = output_buffer;
}

void LED::set_colour_offsets(uint8_t *colour_offsets) {
    this->COLOUR_OFFSETS = colour_offsets;
}
