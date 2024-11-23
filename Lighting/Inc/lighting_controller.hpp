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

// TODO: determine if this builds
// extern static constexpr uint8_t NUM_LEDS;

// TODO: Make these public
void run_lighting_board();

// TODO: Make these private
void initialize_dma_output_buffer(uint8_t *dma_output_buffer, uint8_t *led_bank_output_buffer, uint16_t bank_size);

void temp_make_led_colours(uint8_t state);

class LightingController {
public:
    /**
     * TODO: Initialize lighting controller with a reference to the led bank output
     */
    LightingController(uint8_t *bank_output_buffer);

    /**
     * TODO: re-colour all of the LED's
     */
    void recolour_all(RGB_colour_t desired_colour);

    /**
     * TODO: re-colour LED by index
     */
    void recolour_by_index(uint8_t index, RGB_colour_t desired_colour);

private:
    uint8_t *lc_output_buffer;
    // WS2812 *leds[NUM_LEDS]; TODO: make this work

}
#endif /* INC_LIGHTING_CONTROLLER_HPP_ */
