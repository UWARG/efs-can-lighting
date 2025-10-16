/*
 * rtos.c
 *
 *  Created on: Oct 16, 2025
 *      Author: micah
 */

#include <stdint.h>
#include <stddef.h>
#include "rtos.h"

typedef struct {
    uint32_t *stack_ptr;
    uint32_t stack[RTOS_STACK_SIZE];
    uint8_t state; // Ready (0), Running (1), Blocked (2)
    uint8_t priority;
    uint32_t wake_tick; // Tick to wake up at
} Task;

static Task tasks[RTOS_MAX_TASKS];
static int current_task;
static volatile uint32_t rtos_tick = 0;
enum {READY, RUNNING, BLOCKED};

static void context_switch(uint32_t **current_sp, uint32_t **next_sp) __attribute__((naked));
static void schedule(void);

// Forward declaration for SysTick_Handler
void SysTick_Handler(void);

// Idle Task
void rtos_idle_task(void) {
	while (1) {
		; // Do nothing
	}
}

void rtos_init(void) {
    for (int i = 0; i < RTOS_MAX_TASKS; i++) {
        tasks[i].state = BLOCKED;
        tasks[i].stack_ptr = NULL;
        tasks[i].priority = 0;
    }

    rtos_create_task(&rtos_idle_task, 255); // Idle task
    current_task = 0; // Start with idle task
}

int rtos_create_task(void (*task_func)(void), uint8_t priority) {
    for (int i = 0; i < RTOS_MAX_TASKS; i++) {
        if (tasks[i].stack_ptr == NULL) {
            tasks[i].stack_ptr = &tasks[i].stack[RTOS_STACK_SIZE];
            *(--tasks[i].stack_ptr) = (1U << 24); // xPSR
            *(--tasks[i].stack_ptr) = (uint32_t)task_func; // PC
            *(--tasks[i].stack_ptr) = 0xFFFFFFFD; // LR
            for (int j = 0; j < 5; j++) {
                *(--tasks[i].stack_ptr) = 0; // R12, R3, R2, R1, R0
            }
            for (int j = 0; j < 8; j++) {
                *(--tasks[i].stack_ptr) = 0; // R4-R11
            }
            tasks[i].state = READY;
            tasks[i].priority = priority;
            return 0; // Success
        }
    }
    return -1; // No available task slots
}

void rtos_start(void) {
    SysTick_Handler(); // Initial call to start the first task
}

uint32_t rtos_get_tick(void) {
    return rtos_tick;
}

void rtos_set_tick(uint32_t new_tick) {
    rtos_tick = new_tick;
}

void rtos_delay(uint32_t ticks) {
    tasks[current_task].wake_tick = rtos_tick + ticks;
    tasks[current_task].state = BLOCKED;
    schedule();
}

// Private (Static) Functions
static __attribute__((naked)) void context_switch(uint32_t **current_sp, uint32_t **next_sp)
{
    __asm volatile(
        "PUSH {R4-R11} \n" // Save registers on the current stack
        "STR SP, [R0] \n" // Store current sp in current_sp
        "LDR SP, [R1] \n" // Load next_spn into sp.
        "POP {R4-R11} \n" // Restore registers from the new stack
        "BX LR \n" // Return from function
    );
}

static void schedule(void) {
    int min_priority = 255;
    int next_task = 0;
    for (int i = 0; i< RTOS_MAX_TASKS; i++) {
        if (tasks[i].state == READY && tasks[i].priority < min_priority) {
            min_priority = tasks[i].priority;
            next_task = i;
        }
    }
    if (next_task == current_task) {
    	return; // No other ready tasks
    }
    int old_task = current_task;
    if (tasks[old_task].state != BLOCKED) {
        tasks[old_task].state = READY;
    }
    current_task = next_task;
    tasks[current_task].state = RUNNING;
    context_switch(&tasks[old_task].stack_ptr, &tasks[current_task].stack_ptr);
}

// Interrupt Handlers
void SysTick_Handler(void) {
	for (int i = 0; i < RTOS_MAX_TASKS; i++) {
		if (tasks[i].state == BLOCKED && rtos_tick >= tasks[i].wake_tick) {
			tasks[i].state = READY;
		}
	}
    rtos_tick++;
    HAL_IncTick();
    schedule();
}
