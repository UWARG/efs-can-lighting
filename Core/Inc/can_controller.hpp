/*
 * lighting_controller.hpp
 *
 *  Created on: March 16, 2025
 *      Author: Nick
 */

#ifndef INC_CAN_CONTROLLER_HPP_
#define INC_CAN_CONTROLLER_HPP_

#include <cstdint>
#include "canard.h"
#include <cstdlib>
#include "stm32l4xx_hal.h"
#include <functional>

class CANController {
public:
  static void initialize(
	uint32_t node_id,
	uint8_t *memory_pool,
	std::size_t memory_pool_size,
	CAN_HandleTypeDef *hcan,
	CanardInstance *canard,
	std::function<void(uint8_t)> set_control_state_callback
  );
private:
  /// Initializes, sets interrupts and starts HAL_CAN; Initializes and sets the node ID of canard
  CANController(
	uint32_t node_id,
	uint8_t *memory_pool,
	std::size_t memory_pool_size,
	CAN_HandleTypeDef *hcan,
	CanardInstance *canard,
	std::function<void(uint8_t)> set_control_state_callback
  );
  uint32_t node_id;
  uint8_t *memory_pool;
  std::function<void(uint8_t)> set_control_state_callback;

  CanardInstance canard;

  friend void onTransferReceived(CanardInstance *ins, CanardRxTransfer *transfer);
};

#endif
