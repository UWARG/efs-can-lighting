#include "can_controller.hpp"
#include <dronecan_msgs.h>
#include "canard_stm32_driver.h"
#include <stdio.h>
#include "canard.h"

union MaybeCANController {
	struct {} uninitialized;
	CANController controller;

	MaybeCANController() {}
	~MaybeCANController() {}
} can_controller;

bool can_controller_initialized = false;

void onTransferReceived(CanardInstance *ins, CanardRxTransfer *transfer);
bool shouldAcceptTransfer(
  const CanardInstance *ins,
  uint64_t *out_data_type_signature,
  uint16_t data_type_id,
  CanardTransferType transfer_type,
  uint8_t source_node_id
);

void CANController::initialize(
  uint32_t node_id,
  uint8_t *memory_pool,
  std::size_t memory_pool_size,
  CAN_HandleTypeDef *hcan,
  CanardInstance *canard,
  void (*set_control_state_callback)(uint8_t)
) {
  if (!can_controller_initialized) {
	  can_controller_initialized = true;
	  can_controller.controller = CANController(
	    node_id, memory_pool, memory_pool_size,
		hcan, canard, set_control_state_callback
	  );
  }
}



CANController::CANController(
  uint32_t node_id,
  uint8_t *memory_pool,
  std::size_t memory_pool_size,
  CAN_HandleTypeDef *hcan,
  CanardInstance *canard,
  void (*set_control_state_callback)(uint8_t)
): node_id(node_id), memory_pool(memory_pool), set_control_state_callback(set_control_state_callback) {
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
	
  canardInit(
    canard,
    memory_pool,
    memory_pool_size,
    onTransferReceived,
    shouldAcceptTransfer,
    NULL
  );

  canardSetLocalNodeID(canard, node_id);
}

void onTransferReceived(CanardInstance *ins, CanardRxTransfer *transfer) {
  if (transfer->data_type_id == WARG_SETCONTROLSTATE_SIGNATURE) {
    struct warg_SetControlState control_state;
    if (warg_SetControlState_decode(transfer, &control_state)) {
      return;
    }
    if (!can_controller_initialized) {
    	return;
    }
    (can_controller.controller.set_control_state_callback)(control_state.controlState);
  }
}

bool shouldAcceptTransfer(
  const CanardInstance *ins,
  uint64_t *out_data_type_signature,
  uint16_t data_type_id,
  CanardTransferType transfer_type,
  uint8_t source_node_id
) {
  if (transfer_type == CanardTransferTypeRequest) {
  	// check if we want to handle a specific service request
		switch (data_type_id) {
		case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
			*out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE;
			return true;
		}
		}
  } else if (transfer_type == CanardTransferTypeResponse) {
    switch (data_type_id) {}
  } else if (transfer_type == CanardTransferTypeBroadcast) {
  }
  if (data_type_id == WARG_SETCONTROLSTATE_ID) {
 		// see if we want to handle a specific broadcast packet
		switch (data_type_id) {
		case UAVCAN_PROTOCOL_NODESTATUS_ID: {
			*out_data_type_signature = UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE;
			return true;
		}
		case ARDUPILOT_INDICATION_NOTIFYSTATE_ID: {
			*out_data_type_signature = ARDUPILOT_INDICATION_NOTIFYSTATE_SIGNATURE;
			return true;
		}
		case WARG_SETCONTROLSTATE_ID: {
			*out_data_type_signature = WARG_SETCONTROLSTATE_SIGNATURE;
			return true;
		}
		}   
  }
  return false;
}
