#include "main.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "canard_stm32_driver.h"
#include <time.h>
#include <stdio.h>
#include "../dsdlc_generated/include/dronecan_msgs.h"
#include <lighting_demos.hpp>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define ROTATE_LED
//#define CYCLE_ONE_LED_ON
//#define CONSTANT_COLOR
#include "lighting_controller.hpp"
#include "can_controller.hpp"
#define CLOCK_MONOTONIC 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static CanardInstance canard;
static uint8_t memory_pool[1024];
static struct uavcan_protocol_NodeStatus node_status;
extern TIM_HandleTypeDef htim6;
static uint32_t node_id;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void initializeNodeId() {
	uint8_t buffer[16];
	getUniqueID(buffer);
	uint32_t *parts = (uint32_t *)buffer;
	node_id = parts[0] ^ parts[1] ^ parts[2];
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	// Receiving
	CanardCANFrame rx_frame;

	const uint64_t timestamp = HAL_GetTick() * 1000ULL;
	const int16_t rx_res = canardSTM32Recieve(hcan, CAN_RX_FIFO0, &rx_frame);

	if (rx_res < 0) {
		printf("Receive error %d\n", rx_res);
	}
	else if (rx_res > 0)        // Success - process the frame
	{
		canardHandleRxFrame(&canard, &rx_frame, timestamp);
	}
}

// Processes the canard Tx queue and attempts to transmit the messages
// Call this function very often to check if there are any Tx to process
// Calling it once every cycle of the while(1) loop is not a bad idea
void processCanardTxQueue(CAN_HandleTypeDef *hcan) {
	// Transmitting

	for (const CanardCANFrame *tx_frame ; (tx_frame = canardPeekTxQueue(&canard)) != NULL;) {
		const int16_t tx_res = canardSTM32Transmit(hcan, tx_frame);

		if (tx_res <= 0) {
			printf("Transmit error %d\n", tx_res);
		} else if (tx_res > 0) {
			printf("Successfully transmitted message\n");
		}

		// Pop canardTxQueue either way
		canardPopTxQueue(&canard);
	}
}

/*
  send the 1Hz NodeStatus message. This is what allows a node to show
  up in the DroneCAN GUI tool and in the flight controller logs
 */
void send_NodeStatus(void) {
    uint8_t buffer[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];

    node_status.uptime_sec = HAL_GetTick() / 1000UL;
    node_status.health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK;
    node_status.mode = UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL;
    node_status.sub_mode = 0;

    // put whatever you like in here for display in GUI
    node_status.vendor_specific_status_code = 1234;

    uint32_t len = uavcan_protocol_NodeStatus_encode(&node_status, buffer);

    // we need a static variable for the transfer ID. This is
    // incremeneted on each transfer, allowing for detection of packet
    // loss
    static uint8_t transfer_id;

    canardBroadcast(&canard,
                    UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                    UAVCAN_PROTOCOL_NODESTATUS_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    buffer,
                    len);
}

/*
  This function is called at 1 Hz rate from the main loop.
*/
void process1HzTasks(uint64_t timestamp_usec) {
    /*
      Purge transfers that are no longer transmitted. This can free up some memory
    */
    canardCleanupStaleTransfers(&canard, timestamp_usec);

    /*
      Transmit the node status message
    */
    //send_NodeStatus();
}



void process10HzTasks(uint64_t timestamp_usec) {
    uint8_t buffer[WARG_SETCONTROLSTATE_MAX_SIZE];

    struct warg_SetControlState value;

    value.controlState = 0;

    uint32_t len = warg_SetControlState_encode(&value, buffer);

    // we need a static variable for the transfer ID. This is
    // incremeneted on each transfer, allowing for detection of packet
    // loss
    static uint8_t transfer_id;

    canardBroadcast(&canard,
    				WARG_SETCONTROLSTATE_SIGNATURE,
					WARG_SETCONTROLSTATE_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    buffer,
                    len);

    send_NodeStatus();
}
extern LightingController rev4;

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
	MX_TIM6_Init();
	MX_TIM7_Init();
	MX_TIM2_Init();
//here

  /* USER CODE BEGIN 2 */

	HAL_TIM_Base_Start_IT(&htim6);

	rev4.start_lighting_control(); //start lighting
	uint8_t all_domains_enabled = (1 << 7);
	rev4.configure_allowed_domains(all_domains_enabled);

	//set up the domain colours and brightness
	rev4.set_domain_colour_and_brightness(CD_MAIN, PURPLE, 15);
	rev4.set_domain_colour_and_brightness(CD_TAXI, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_LANDING, WHITE, 15);
	rev4.set_domain_colour_and_brightness(CD_NAV, GREEN, 15);
	rev4.set_domain_colour_and_brightness(CD_BEACON, RED, 15); //CHANGE THIS TO RED.
	rev4.set_domain_colour_and_brightness(CD_STROBE, ORANGE, 15);
	rev4.set_domain_colour_and_brightness(CD_BRAKE, ORANGE, 15);
	rev4.set_domain_colour_and_brightness(CD_SEARCH, WHITE, 15);

	rev4.configure_active_domains(255);

	//Declare control states
	LC_State_STARTUP startup_state;
	LC_State_GROUND ground_state;
	LC_State_FLIGHT flight_state;
	LC_State_BRAKE brake_state;
	LC_State_LANDING landing_state;
	LC_State_STANDBY standby_state;
	LC_State_SEARCH search_state;


	auto set_control_state = [&](uint8_t state) {
		switch (state) {
		case TRANSITION_GROUND: {
			rev4.set_lighting_control_state(&ground_state);
			break;
		}
		case TRANSITION_STANDBY: {
			rev4.set_lighting_control_state(&standby_state);
			break;

		}
		default: {
			break;

		}
		}
	};
  initializeNodeId();
  CANController::initialize(
  	node_id, memory_pool, 1024,
  	&hcan1, &canard, set_control_state
  );
	uint64_t next_1hz_service_at = HAL_GetTick();
	uint64_t next_10hz_service_at = HAL_GetTick();


	// Starts the 1s pulse asap (no weird user setup calls).
	// I don't think this changes timing at all but maybe it does.


	//allow all control domains.


//  lighting_control_state_demo();
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		const uint64_t ts = HAL_GetTick();

		if (ts >= next_1hz_service_at){
		  next_1hz_service_at += 1000ULL;
		  process1HzTasks(ts);
		}

		if (ts >= next_10hz_service_at) {
			next_10hz_service_at += 100ULL;
			process10HzTasks(ts);
		}

		processCanardTxQueue(&hcan1);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
