#pragma once

#include <can.h>
#include "dma.hpp"
#include "gpio.hpp"
#include "ws2812.hpp"
#include "main.h"
#include <time.h>
#include <stdio.h>
#include <canard.h>
#include <string.h>
#include <tim.h>
#include "node_settings.hpp"
#include "dronecan_msgs.h"
#include "canard_stm32_driver.hpp"

#define CLOCK_MONOTONIC 1

// static vs extern?
extern CanardInstance canard;
extern uint8_t memory_pool[1024];
extern struct uavcan_protocol_NodeStatus node_status;

void handle_lights_command(CanardInstance *ins, CanardRxTransfer *transfer);
void set_rgb_led(uint8_t red, uint8_t green, uint8_t blue);

// Custom clock_gettime function for STM32
int clock_gettime(clockid_t clk_id, struct timespec *tp);
void getUniqueID(uint8_t id[16]);

void HAL_CAN_RxFifo0Callback(CAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);

void handle_NodeStatus(CanardInstance *ins, CanardRxTransfer *transfer);

void handle_NotifyState(CanardInstance *ins, CanardRxTransfer *transfer);


void handle_GetNodeInfo(CanardInstance *ins, CanardRxTransfer *transfer);

void send_NodeStatus(void);

extern volatile uint64_t last_frame_timestamp_usec;

extern volatile bool shouldFlashLights;

void onTransferReceived(CanardInstance *ins, CanardRxTransfer *transfer);

bool shouldAcceptTransfer(const CanardInstance *ins,
                            uint64_t *out_data_type_signature,
                            uint16_t data_type_id,
                            CanardTransferType transfer_type,
                            uint8_t source_node_id);

void processCanardTxQueue(CAN_HandleTypeDef *hfdcan);
void process1HzTasks(uint64_t timestamp_usec);

int16_t constrain_int16(int16_t value, int16_t min, int16_t max);

void handle_lights_command(CanardInstance *ins, CanardRxTransfer *transfer);

void set_rgb_led(uint8_t red, uint8_t green, uint8_t blue);
