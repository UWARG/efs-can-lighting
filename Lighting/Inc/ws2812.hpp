/*
 * ws2812.hpp
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni
 */

#ifndef INC_WS2812_HPP_
#define INC_WS2812_HPP_

static constexpr uint8_t PWM_LO = 19;
static constexpr uint8_t PWM_HI = 38;

void initialize_bank_output_buffer_off(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad);
void initialize_bank_output_buffer_on(uint8_t *bank_out_buff, uint8_t num_led, uint8_t num_pad);


#endif /* INC_WS2812_HPP_ */
