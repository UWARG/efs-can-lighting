/*
 * lighting_controller.hpp
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni
 */

#ifndef INC_LIGHTING_CONTROLLER_HPP_
#define INC_LIGHTING_CONTROLLER_HPP_

// TEMPORARY VALUES

// TODO: Make these public
void run_lighting_board();

// TODO: Make these private
void initialize_dma_output_buffer(uint8_t *dma_output_buffer, uint8_t *led_bank_output_buffer, uint16_t bank_size);

void temp_make_led_colours(uint8_t state);
#endif /* INC_LIGHTING_CONTROLLER_HPP_ */
