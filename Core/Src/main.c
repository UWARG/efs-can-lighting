/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ws2812.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define ROTATE_LED
//#define CYCLE_ONE_LED_ON
#define CONSTANT_COLOR
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	uint8_t angle = 0;
	const uint8_t angle_difference = 11;
	led_buffer_init();
//
//	int rotate_lo = 8;
//	int rotate_hi = 16;

//	uint8_t num_led = 6; // Number of LED's we have
	uint8_t led_in_cycle = 0;	// This is a value that we use to keep track of which LED is on during the cycle.
//	num_led += 1; // +1 because we need 24 bits of 0 at the start
//	uint8_t buff_base = num_led * 24; // Each LED has 24 Bits (8 for each of RGB)
//	uint8_t out_buf[buff_base + 24]; // We add 24 to the end so that we have 24 (one LED) bits of 0
//	uint8_t setval = PWM_LO;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		// Demo code for 8 LEDs
//		for (uint8_t i = 0; i < NUM_PIXELS /* Change that to your amount of LEDs */;
//				i++) {
//			// Calculate color
//			uint32_t rgb_color = hsl_to_rgb(angle + (i * angle_difference), 255,
//					127);
//			// Set color
//			led_set_RGB(i, (rgb_color >> 16) & 0xFF, (rgb_color >> 8) & 0xFF,
//					rgb_color & 0xFF);
//		}
//		// Write to LED
//		++angle;
//		led_render();
//		// Some delay
//		HAL_Delay(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		led_set_RGB(0, 0, 0, 0);
//		led_set_RGB(1, 0, 0, 0);
//		led_set_RGB(0, 50, 0, 0);
//		led_set_RGB(1, 50, 0, 0);
//		led_set_RGB(2, 50, 0, 0);
//		led_render();
//
//		HAL_Delay(1000);
//		led_set_RGB(0, 0, 50, 0);
//	    led_set_RGB(1, 0, 50, 0);
//		led_set_RGB(2, 0, 50, 0);
//		led_render();
//		HAL_Delay(1000);
//
//		led_set_RGB(0, 0, 0, 50);
//		led_set_RGB(1, 0, 0, 50);
//		led_set_RGB(2, 0, 0, 50);
//		led_render();
//		HAL_Delay(1000);
		// Every loop, we set the bytes that we want to send out directly
//# ifdef ROTATE_LED
//		for (int i= 0; i < 24; i++) {
//			out_buf[i] = 0; // set the fisrt 24 bits to 0
//			for (int j= 1; j < num_led; j++) {
//			if ((i < rotate_hi) && (i >= rotate_lo) && ((i%8) >5)) {
//				out_buf[i+j*24] = PWM_HI;
////				out_buf[i+24] = PWM_HI;
//			} else {
//				out_buf[i+j*24] = PWM_LO;
////				out_buf[i+24] = PWM_LO;
//			}
//			}
//		}
//
//		rotate_lo += 8;
//		rotate_hi += 8;
//
//		if (rotate_lo == 24) {
//			rotate_lo = 0;
//			rotate_hi = 8;
//		}
//# endif

//#ifdef CYCLE_ONE_LED_ON
//		uint8_t led_that_should_be_on = led_in_cycle;
//		/**
//		 * Cycles one LED on for each cycle
//		 */
//
//		// This for loop is kinda busted, the nesting can go either way.
//		// Right now, it iterates through every individual position of the 24-bit colours
//		// Recall j=0 is the dead LED
//		// We are only assigning J=1-6, where num_led = 6+1 (dead LED accounted for at the0th index)
////		for (int i = 0; i < 24; i++) {
////			out_buf[i] = 0;
////			for (int j = 1; j < num_led; j++) {
////				if (j == led_in_cycle) {
////					// This LED needs to be on
////					if ((i % 8) > 5) {
////						out_buf[i + j * 24] = PWM_HI;
////					} else {
////						out_buf[i + j * 24] = PWM_LO;
////					}
//////				} else if (j == (led_in_cycle + 1)) { // Only use this if you want goofiness
////					// Basically forces the next "led" in sequence to be a flat line, instead of a neopixel command
//////					out_buf[i+j*24] = 0;
////				} else {
////					// This LED needs to be off
////					out_buf[i + j * 24] = PWM_LO;
////				}
////			}
////		}
//
//
//		led_buffer_init();
//		led_set_RGB_index(led_in_cycle, 0x000404);
//
//
////
////		// Add 1 and wrap around if it's too large
//		led_in_cycle += 1;
//		if (led_in_cycle > 6) {
//			led_in_cycle = 1;
//		}
//#endif

//
//#ifdef CONSTANT_COLOR
//		for (int i= 0; i < 24; i++) {
//			out_buf[i] = 0; // set the fisrt 24 bits to 0
//			for (int j= 1; j < num_led; j++) {
//				if ((i%8) == 6 && i > 7) {
//					//Just set the first bit of red and blue bytes high to get purple.
//					out_buf[i+j*24] = PWM_HI;
//		//				out_buf[i+24] = PWM_HI;
//				} else {
//					out_buf[i+j*24] = PWM_LO;
//		//			out_buf[i+24] = PWM_LO;
//				}
//			}
//		}
//#endif

//		// No matter what operation we do, we set the last bits in the buffer to 0;
//		for (int i = buff_base; i < buff_base + 24; i++) { // TODO: Make this 24 a variable "zero_extend_length"
//			out_buf[i] = 0;
//		}

//		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
//		HAL_Delay(20);



//		led_buffer_init();
//		led_set_RGB_index(0, 0x000404);
		led_set_all_RGBs(0x000404);
		HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*) out_buf,
						BUFF_SIZE);
//		led_render_RGB();
//		led_set_all_RGBs(0, 0x000404);


		HAL_Delay(1000);
//		HAL_Delay(100);

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
