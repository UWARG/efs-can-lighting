/*
 * dronecan_node.h
 *
 *  Created on: Oct 7, 2025
 *      Author: micah
 */

#ifndef DRONECAN_NODE_H
#define DRONECAN_NODE_H

#include "stm32l4xx_hal_conf.h"
#include "canard.h"

#ifdef __cplusplus
extern "C" {
#endif

extern CanardInstance canard;
extern uint8_t memory_pool[1024];

void dronecan_on_can_rx(CAN_HandleTypeDef *hcan);
void process1HzTasks();
bool shouldAcceptTransfer(const CanardInstance *ins,
                          uint64_t *out_data_type_signature,
                          uint16_t data_type_id,
                          CanardTransferType transfer_type,
                          uint8_t source_node_id);
void onTransferReceived(CanardInstance *ins, CanardRxTransfer *transfer);

#ifdef __cplusplus
}
#endif


#endif
