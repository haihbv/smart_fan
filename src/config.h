#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>

// ============================================================================
// MCP CONFIG
// ============================================================================

#define MCP_ENDPOINT "wss://api.xiaozhi.me/mcp/?token=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VySWQiOjkzMzM5OCwiYWdlbnRJZCI6MTg0OTYzNiwiZW5kcG9pbnRJZCI6ImFnZW50XzE4NDk2MzYiLCJwdXJwb3NlIjoibWNwLWVuZHBvaW50IiwiaWF0IjoxNzc5MTcxMDMzLCJleHAiOjE4MTA3Mjg2MzN9.cVNymqwvIwCBSmppU65Z3nJ38Y_C-YNcgIpbBgqzpOiJSFRwtYvtbOnq3b3PmiFtVuT7ez9vHXPVpz-b5LctPA" ///< MCP WebSocket endpoint
#define WIFI_AP_SSID "SMART FAN"                                                                                                                                                                                                                                                                                                                        ///< WiFi AP SSID
#define WIFI_AP_PASS "66668888"                                                                                                                                                                                                                                                                                                                         ///< WiFi AP password

#define WIFI_RECONNECT_INTERVAL_MS 5000 ///< WiFi reconnect interval
#define MCP_WATCHDOG_TIMEOUT_MS 60000   ///< MCP watchdog timeout
#define STACK_LOG_INTERVAL_MS 30000     ///< Stack log interval

// ============================================================================
// IR CONFIG
// ============================================================================

#define IR_SEND_PIN 4           ///< IR transmit pin
#define IR_FAN_POWER 0xE31CFF00 ///< IR code: fan power
#define IR_FAN_SPEED 0xE916FF00 ///< IR code: fan speed

// ============================================================================
// MOTOR CONFIG
// ============================================================================

#define IN1 25 ///< Left motor IN1
#define IN2 26 ///< Left motor IN2
#define IN3 27 ///< Right motor IN1
#define IN4 14 ///< Right motor IN2

#define ENA 33 ///< Left motor PWM
#define ENB 12 ///< Right motor PWM

#define CH1 0 ///< LEDC channel ENA
#define CH2 1 ///< LEDC channel ENB

#define MOTOR_SPEED_FULL 255 ///< Full speed
#define MOTOR_SPEED_TURN 220 ///< Turn speed

#define MOVE_DURATION_MS 5000 ///< Move duration
// #define TURN_DURATION_MS 2000 ///< Turn duration
#define TURN_DURATION_MS 330 // quay 15 độ

#define FAN_SWING_STEP_MS 2500   ///< Swing step time
#define FAN_SWING_RETURN_MS 5000 ///< Swing return time
#define MOTOR_STEP_CHECK_MS 50   ///< Motor check interval

#define FAN_SWING_LEFT -1  ///< Swing left
#define FAN_SWING_CENTER 0 ///< Swing center
#define FAN_SWING_RIGHT 1  ///< Swing right

// ============================================================================
// FREERTOS CONFIG
// ============================================================================

#define MCP_TASK_CORE 0   ///< MCP task core
#define MOTOR_TASK_CORE 1 ///< Motor task core
#define IR_TASK_CORE 1    ///< IR task core

#define MCP_TASK_PRIORITY 5   ///< MCP task priority
#define MOTOR_TASK_PRIORITY 3 ///< Motor task priority
#define IR_TASK_PRIORITY 2    ///< IR task priority

#define MCP_TASK_STACK 4096   ///< MCP task stack
#define MOTOR_TASK_STACK 2048 ///< Motor task stack
#define IR_TASK_STACK 2048    ///< IR task stack

#define MOTOR_CMD_QUEUE_SIZE 5 ///< Motor queue size
#define IR_CMD_QUEUE_SIZE 5    ///< IR queue size

#endif /* _CONFIG_H_ */