/*
 * rtos.h
 *
 *  Created on: Oct 16, 2025
 *      Author: micah
 */

#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>

#define RTOS_MAX_TASKS 5
#define RTOS_STACK_SIZE 64
#define RTOS_TICK_HZ 1000U // 1ms tick
#define RTOS_LEAST_PRIORITY 255 // Lowest priority
#define RTOS_LR 0xFFFFFFFD // EXC_RETURN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the RTOS.
 */
void rtos_init(void);

/**
 * Creates a new task.
 * @param task_func Pointer to the task function.
 * @param priority Task priority (0 = highest).
 * @return 0 on success, -1 on failure.
 */
int rtos_create_task(void (*task_func)(void), uint8_t priority);

/**
 * Starts the RTOS scheduler.
 */
void rtos_start(void);

/**
 * Returns the current RTOS tick value.
 * @return Current tick value.
 */
uint32_t rtos_get_tick(void);

/**
 * Sets the RTOS tick value.
 * @param new_tick New tick value to set.
 */
void rtos_set_tick(uint32_t new_tick);

/**
 * Delays the current task for a specified number of ticks.
 * @param ticks Number of ticks to delay.
 */
void rtos_delay(uint32_t ticks);

/**
 * Task function for ground breathe effect.
 * @param state Current control state.
 */
void ground_state_breathe_task(void);

/**
 * Task function for processing 1Hz tasks.
 */
void process_1hz_task(void);

/**
 * Task function for setting control state.
 */
void set_control_state_task(void);

/**
 * Calculates the next control state.
 */
void calculate_next_state_task(void);

#ifdef __cplusplus
}
#endif

#endif // RTOS_H
