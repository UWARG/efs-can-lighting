/*
 * lighting_controller.cpp
 *
 * This file handles all of the higher level logic / interfacing to the can board. This might be a class as well.
 *
 * It creates 6 instances of the WS2812 LED's
 *
 * It handles the final output buffer that is sent to DMA, as well as DMA transfer half complete callback
 *
 *  Created on: Nov 21, 2024
 *      Author: Anni
 */

// Use uint8_t instead of size_t to ensure we don't get too big
static constexpr uint8_t NUM_LEDS = 6;			// Number of LED's on one board
static constexpr uint8_t NUM_LEDS_PADDING = 1;	// Number of LED's worth of padding (0) to give
static constexpr uint8_t DMA_OUTPUT_BUFFER_SIZE = (NUM_LEDS + NUM_LEDS_PADDING*2) * 24 * 2;	// 24 bits per LED, (or padding), x2 for DMA functionality

// TODO: Replace this with something better???? Not sure what yet
static constexpr uint8_t LED_BANK_OUTPUT_BUFFER_SIZE = (NUM_LEDS + NUM_LEDS_PADDING*2) *24; // Padding for one bank of LED's, + padding

// TODO: define custom types so we don't need this weirdness
uint8_t out_buf[DMA_OUTPUT_BUFFER_SIZE];

// CALLBACKS
// TODO: Register custom callbacks for the timers
// https://community.st.com/t5/stm32-mcus/how-to-use-register-callbacks-in-stm32/ta-p/580499
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//          ^ Current location
	// So update BANK 1
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	// | BANK 1 | BANK 2 |
	//                   ^ Current location
	// So update BANK 2

}
