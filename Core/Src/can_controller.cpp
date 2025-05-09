#include "can_controller.hpp"
#include <stdio.h>

union MaybeCANController {
	struct {} uninitialized;
	CANController controller;

	MaybeCANController() {}
	~MaybeCANController() {}
} can_controller;

bool can_controller_initialized = false;

void CANController::initialize(
  uint32_t node_id,
  CAN_HandleTypeDef *hcan,
  std::function<void(uint8_t)> set_control_state_callback
) {
  if (!can_controller_initialized) {
	  can_controller_initialized = true;
	  can_controller.controller = CANController(
	    node_id, hcan, set_control_state_callback
	  );
  }
}

// likely called as CANController::onTransferReceived(&hcan1, CAN_RX_FIFO0)
void CANController::onTransferReceived(CAN_HandleTypeDef *hcan, uint32_t rxFifoIndex) {
	CAN_RxHeaderTypeDef frameHeader;
	uint8_t frameData[8];

	if (HAL_CAN_GetRxMessage(hcan, rxFifoIndex, &frameHeader, frameData) != HAL_OK) return;
	if (frameHeader.IDE != CAN_ID_STD) return;
	if (frameHeader.RTR != CAN_RTR_DATA) return;
	if (frameHeader.DLC != 1) return;

	uint8_t state = frameData[0];

	if (!can_controller_initialized) return;
	(can_controller.controller.set_control_state_callback)(state);
}

CANController::CANController(
  uint32_t node_id,
  CAN_HandleTypeDef *hcan,
  std::function<void(uint8_t)> set_control_state_callback
): node_id(node_id), set_control_state_callback(set_control_state_callback) {
  CAN_FilterTypeDef filter;

  	filter.FilterBank = 0;
	filter.FilterMode = CAN_FILTERMODE_IDMASK;
	filter.FilterFIFOAssignment = CAN_RX_FIFO0;
	filter.FilterIdHigh = 0;
	filter.FilterIdLow = 0;
	filter.FilterMaskIdHigh = 0;
	filter.FilterMaskIdLow = 0;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;
	filter.FilterActivation = ENABLE;
	filter.SlaveStartFilterBank = 14;  

	HAL_CAN_ConfigFilter(hcan, &filter);
	HAL_CAN_Start(hcan);
	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}
