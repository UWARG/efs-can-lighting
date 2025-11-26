
#include <cstdint>
#include <cstring>

#include "ws2812.hpp"


WS2812::WS2812() : LED(colour_offsets, values_to_write, NUM_CHANNELS) {
	// GRB
	colour_offsets[GREEN] = 0;
	colour_offsets[RED] = 8;
	colour_offsets[BLUE] = 16;
}

WS2812::WS2812(uint8_t *output_buffer) : WS2812() {
	this->buffer = output_buffer;
	initialize_led_off(output_buffer);
}

void WS2812::set_led_colour(RGB_colour_t rgb_colour_value) {
	this->colour = rgb_colour_value;
	RGB_colour_t rgb_compensated_colour = this->colour;

	rgb_compensated_colour.green = this->colour.green
			* ((float) this->brightness / 100.0);
	rgb_compensated_colour.red = this->colour.red
			* ((float) this->brightness / 100.0);
	rgb_compensated_colour.blue = this->colour.blue
			* ((float) this->brightness / 100.0);
	
	this->colour = rgb_compensated_colour;
	convert_colour_to_value();
	push_colour_to_output_buffer();
    this->colour = rgb_colour_value;
}


void WS2812::set_brightness(uint8_t colour_brightness) {
	this->brightness = colour_brightness;
	RGB_colour_t rgb_compensated_colour = this->colour;

	rgb_compensated_colour.green = this->colour.green
			* ((float) colour_brightness / 100.0);
	rgb_compensated_colour.red = this->colour.red
			* ((float) colour_brightness / 100.0);
	rgb_compensated_colour.blue = this->colour.blue
			* ((float) colour_brightness / 100.0);
	

    RGB_colour_t saved_colour = this->colour;
	this->colour = rgb_compensated_colour;

	convert_colour_to_value();
	push_colour_to_output_buffer();
    this->colour = saved_colour;
}


void WS2812::convert_colour_to_value() {
	for (int i = 0; i < BITS_PER_COLOUR; ++i) {
		if ((colour.green >> i) & 0x1) {
			values_to_write[colour_offsets[GREEN] - i + BITS_PER_COLOUR - 1] = PWM_HI;
		} else {
			values_to_write[colour_offsets[GREEN] - i + BITS_PER_COLOUR - 1] = PWM_LO;
		}

		if ((colour.red >> i) & 0x1) {
			values_to_write[colour_offsets[RED] - i + BITS_PER_COLOUR - 1] = PWM_HI;
		} else {
			values_to_write[colour_offsets[RED] - i + BITS_PER_COLOUR - 1] = PWM_LO;
		}

		if ((colour.blue >> i) & 0x1) {
			values_to_write[colour_offsets[BLUE] - i + BITS_PER_COLOUR - 1] = PWM_HI;
		} else {
			values_to_write[colour_offsets[BLUE] - i + BITS_PER_COLOUR - 1] = PWM_LO;
		}
	}
}



