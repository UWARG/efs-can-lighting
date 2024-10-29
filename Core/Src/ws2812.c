/*
 * ws2812.c
 *
 *  Created on: Sep 22, 2024
 *      Author: anthony
 */

#include "stm32l4xx_hal.h"
#include "ws2812.h"
#include "hex_to_rgb_conversion.h"

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_ch2;

uint8_t led_in_cycle = 0;	// This is a value that we use to keep track of which LED is on during the cycle.
uint8_t out_buf[BUFF_SIZE]; // We add 24 to the end so that we have 24 (one LED) bits of 0
//
//
//// LED color buffer
//uint8_t rgb_arr[NUM_BYTES];
//
//// LED write buffer
//#define WR_BUF_LEN (NUM_BPP * NUM_PIXELS * 8)
//uint8_t wr_buf[WR_BUF_LEN];
//uint_fast8_t wr_buf_p = 0;
//
//static inline uint8_t scale8(uint8_t x, uint8_t scale) {
//  return ((uint16_t)x * scale) >> 8;
//}
//
//// Set a single color (RGB) to index
//void led_set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
//#if (NUM_BPP == 4) // SK6812
//  rgb_arr[4 * index] = scale8(g, 0xB0); // g;
//  rgb_arr[4 * index + 1] = r;
//  rgb_arr[4 * index + 2] = scale8(b, 0xF0); // b;
//  rgb_arr[4 * index + 3] = 0;
//#else // WS2812B
//  rgb_arr[3 * index] = scale8(g, 0xB0); // g;
//  rgb_arr[3 * index + 1] = r;
//  rgb_arr[3 * index + 2] = scale8(b, 0xF0); // b;
//#endif // End SK6812 WS2812B case differentiation
//}
//
//// Set a single color (RGBW) to index
//void led_set_RGBW(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
//  led_set_RGB(index, r, g, b);
//#if (NUM_BPP == 4) // SK6812
//  rgb_arr[4 * index + 3] = w;
//#endif // End SK6812 WS2812B case differentiation
//}
//
//void led_set_all_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
//  for(uint_fast8_t i = 0; i < NUM_PIXELS; ++i) led_set_RGBW(i, r, g, b, w);
//}
//
//// Shuttle the data to the LEDs!
//void led_render() {
//  for(uint_fast8_t i = 0; i < NUM_PIXELS; ++i) {
//	  // For every pixel, yeet
////    wr_buf[i     ] = PWM_LO << (((rgb_arr[0]) & 0x80) > 0);
////    wr_buf[i +  8] = PWM_LO << (((rgb_arr[1]) & 0x80) > 0);
////    wr_buf[i + 16] = PWM_LO << (((rgb_arr[2]) & 0x80) > 0);
//	wr_buf[i     ] = PWM_LO << 0x80;
//	wr_buf[i +  8] = PWM_LO << 0x80;
//	wr_buf[i + 16] = PWM_LO << 0x80;
//    wr_buf[i + 24] = PWM_LO << (((rgb_arr[3]) & 0x80) > 0);
//    wr_buf[i + 32] = PWM_LO << (((rgb_arr[4]) & 0x80) > 0);
//    wr_buf[i + 40] = PWM_LO << (((rgb_arr[5]) & 0x80) > 0);
//  }
//
//  HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)wr_buf, WR_BUF_LEN);
////  wr_buf_p = 2; // Since we're ready for the next buffer
//}


void led_buffer_init() {
	for (int i = 0; i < BUFF_SIZE; i++) {
		if (i < 24 || i > (NUM_LEDS + 1) * 24) {
			out_buf[i] = 0;
		} else {
			out_buf[i] = PWM_LO;
		}

	}
}

void led_set_RGB_index(uint8_t index, uint32_t color_code) {
	//Get the brightness of each color
	uint8_t green_brightness = get_green_from_hex(color_code);
	uint8_t red_brightness = get_red_from_hex(color_code);
	uint8_t blue_brightness = get_blue_from_hex(color_code);

	uint8_t mask = 1;

	for (int g = 7; g >= 0; g--) {
		if (green_brightness & mask) {
			out_buf[g + (index+1)*24] = PWM_HI;
		} else {
			out_buf[g + (index+1)*24] = PWM_LO;
		}
		mask = mask << 1;
	}

	mask = 1;

	for (int r = 7; r >= 0; r--) {
		if (red_brightness & mask) {
			out_buf[r + (index +1)*24 + 8] = PWM_HI;
		} else {
			out_buf[r + (index+1)*24 + 8] = PWM_LO;
		}
		mask = mask << 1;
	}

	mask = 1;

	for (int b = 7; b >= 0; b--) {
		if (blue_brightness & mask) {
			out_buf[b + (index +1)*24 + 16] = PWM_HI;
		} else {
			out_buf[b + (index +1)*24 + 16] = PWM_LO;
		}
		mask = mask << 1;
	}

}

//set the color for all the LEDS
void led_set_all_RGBs(uint32_t color_code) {
	for (int i = 0; i < NUM_LEDS; i++) {
		led_set_RGB_index(i, color_code);
	}
}

void led_render_RGB() {
	//As long as you're not cycling through the LEDs.
	HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) out_buf,
					BUFF_SIZE);
}


//void led_render_cycle(uint32_t color_code) {
//	for (int i = 0; i < NUM_LEDS; i++) {
//		led_set_RGB_index(i, color_code);
//
//	}
//
//}
//

//void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
//  // DMA buffer set from LED(wr_buf_p) to LED(wr_buf_p + 1)
//  if(wr_buf_p < NUM_PIXELS) {
//    // We're in. Fill the even buffer
//#if (NUM_BPP == 4) // SK6812
//    for(uint_fast8_t i = 0; i < 8; ++i) {
//      wr_buf[i     ] = PWM_LO << (((rgb_arr[4 * wr_buf_p    ] << i) & 0x80) > 0);
//      wr_buf[i +  8] = PWM_LO << (((rgb_arr[4 * wr_buf_p + 1] << i) & 0x80) > 0);
//      wr_buf[i + 16] = PWM_LO << (((rgb_arr[4 * wr_buf_p + 2] << i) & 0x80) > 0);
//      wr_buf[i + 24] = PWM_LO << (((rgb_arr[4 * wr_buf_p + 3] << i) & 0x80) > 0);
//    }
//#else // WS2812B
//    for(uint_fast8_t i = 0; i < 8; ++i) {
//      wr_buf[i     ] = PWM_LO << (((rgb_arr[3 * wr_buf_p    ] << i) & 0x80) > 0);
//      wr_buf[i +  8] = PWM_LO << (((rgb_arr[3 * wr_buf_p + 1] << i) & 0x80) > 0);
//      wr_buf[i + 16] = PWM_LO << (((rgb_arr[3 * wr_buf_p + 2] << i) & 0x80) > 0);
//    }
//#endif // End SK6812 WS2812B case differentiation
//    wr_buf_p++;
//  } else if (wr_buf_p < NUM_PIXELS + 2) {
//    // Last two transfers are resets. SK6812: 64 * 1.25 us = 80 us == good enough reset
//  	//                               WS2812B: 48 * 1.25 us = 60 us == good enough reset
//    // First half reset zero fill
//    for(uint8_t i = 0; i < WR_BUF_LEN / 2; ++i) wr_buf[i] = 0;
//    wr_buf_p++;
//  }
//}
//
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
//  // DMA buffer set from LED(wr_buf_p) to LED(wr_buf_p + 1)
//  if(wr_buf_p < NUM_PIXELS) {
//    // We're in. Fill the odd buffer
//#if (NUM_BPP == 4) // SK6812
//    for(uint_fast8_t i = 0; i < 8; ++i) {
//      wr_buf[i + 32] = PWM_LO << (((rgb_arr[4 * wr_buf_p    ] << i) & 0x80) > 0);
//      wr_buf[i + 40] = PWM_LO << (((rgb_arr[4 * wr_buf_p + 1] << i) & 0x80) > 0);
//      wr_buf[i + 48] = PWM_LO << (((rgb_arr[4 * wr_buf_p + 2] << i) & 0x80) > 0);
//      wr_buf[i + 56] = PWM_LO << (((rgb_arr[4 * wr_buf_p + 3] << i) & 0x80) > 0);
//    }
//#else // WS2812B
//    for(uint_fast8_t i = 0; i < 8; ++i) {
//      wr_buf[i + 24] = PWM_LO << (((rgb_arr[3 * wr_buf_p    ] << i) & 0x80) > 0);
//      wr_buf[i + 32] = PWM_LO << (((rgb_arr[3 * wr_buf_p + 1] << i) & 0x80) > 0);
//      wr_buf[i + 40] = PWM_LO << (((rgb_arr[3 * wr_buf_p + 2] << i) & 0x80) > 0);
//    }
//#endif // End SK6812 WS2812B case differentiation
//    wr_buf_p++;
//  } else if (wr_buf_p < NUM_PIXELS + 2) {
//    // Second half reset zero fill
//    for(uint8_t i = WR_BUF_LEN / 2; i < WR_BUF_LEN; ++i) wr_buf[i] = 0;
//    ++wr_buf_p;
//  } else {
//    // We're done. Lean back and until next time!
//    wr_buf_p = 0;
//    HAL_TIMEx_PWMN_Stop_DMA(&htim1, TIM_CHANNEL_2);
//  }

//	HAL_TIMEx_PWMN_Stop_DMA(&htim1, TIM_CHANNEL_2);
////	HAL_Delay(100);
//	TIM2->CCR2 = TIM2->ARR;
////	TIM2->ARR = 100;
//	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}
