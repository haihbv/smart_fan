#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>

// ============================================================================
// MCP CONFIG
// ============================================================================
#define MCP_ENDPOINT                                                 \
  "wss://api.xiaozhi.me/mcp/"                                        \
  "?token=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9."                     \
  "eyJ1c2VySWQiOjkzMzM5OCwiYWdlbnRJZCI6MTg0OTYzNiwiZW5kcG9pbnRJZCI6" \
  "ImFnZW50XzE4NDk2MzYiLCJwdXJwb3NlIjoibWNwLWVuZHBvaW50IiwiaWF0Ijox" \
  "Nzc5MTcxMDMzLCJleHAiOjE4MTA3Mjg2MzN9.cVNymqwvIwCBSmppU65Z3nJ38Y_" \
  "C-YNcgIpbBgqzpOiJSFRwtYvtbOnq3b3PmiFtVuT7ez9vHXPVpz-b5LctPA"

#define WIFI_AP_SSID "SMART FAN" ///< SSID mặc định của WiFiManager
#define WIFI_AP_PASS "66668888"  ///< Mật khẩu mặc định của WiFiManager

#define WIFI_RECONNECT_INTERVAL_MS 5000 ///< Chu kỳ thử kết nối lại WiFi (ms)
#define MCP_WATCHDOG_TIMEOUT_MS 60000   ///< Timeout watchdog MCP (ms)
#define STACK_LOG_INTERVAL_MS 30000     ///< Chu kỳ log stack (ms)

// ============================================================================
// IR CONFIG
// ============================================================================
#define IR_SEND_PIN 4           ///< Chân GPIO phát tín hiệu hồng ngoại
#define IR_FAN_POWER 0xE31CFF00 ///< Mã HEX hồng ngoại điều khiển Bật/Tắt quạt
#define IR_FAN_SPEED \
  0xE916FF00 ///< Mã HEX hồng ngoại điều chỉnh tốc độ quạt (1->8->1)

// ============================================================================
// MOTOR CONFIG
// ============================================================================
#define IN1 25 ///< Chân điều khiển hướng động cơ Trái (1)
#define IN2 26 ///< Chân điều khiển hướng động cơ Trái (2)
#define IN3 27 ///< Chân điều khiển hướng động cơ Phải (1)
#define IN4 14 ///< Chân điều khiển hướng động cơ Phải (2)
#define ENA 33 ///< Chân PWM điều khiển tốc độ động cơ Trái
#define ENB 12 ///< Chân PWM điều khiển tốc độ động cơ Phải
#define CH1 0  ///< Kênh LEDC cho chân ENA
#define CH2 1  ///< Kênh LEDC cho chân ENB

#define MOTOR_SPEED_FULL 255 ///< Tốc độ tối đa khi đi thẳng/lùi
#define MOTOR_SPEED_TURN 220 ///< Tốc độ khi rẽ trái/phải

#define MOVE_DURATION_MS 5000 ///< Thời gian di chuyển thẳng/lùi mặc định (ms)
#define TURN_DURATION_MS 2000 ///< Thời gian rẽ trái/phải mặc định (ms)

#define FAN_SWING_STEP_MS 2500   ///< Thời gian xoay trái/phải một bước (ms)
#define FAN_SWING_RETURN_MS 5000 ///< Thời gian xoay ngược để về giữa (ms)
#define MOTOR_STEP_CHECK_MS 50   ///< Bước kiểm tra timeout động cơ (ms)

#define FAN_SWING_LEFT -1  ///< Giá trị hướng xoay trái
#define FAN_SWING_CENTER 0 ///< Giá trị hướng ở giữa
#define FAN_SWING_RIGHT 1  ///< Giá trị hướng xoay phải

// ============================================================================
// FREERTOS CONFIG
// ============================================================================
#define MCP_TASK_CORE 0   ///< Core chạy MCP
#define MOTOR_TASK_CORE 1 ///< Core chạy motor
#define IR_TASK_CORE 1    ///< Core chạy IR

#define MCP_TASK_PRIORITY 5   ///< Ưu tiên task MCP
#define MOTOR_TASK_PRIORITY 3 ///< Ưu tiên task motor
#define IR_TASK_PRIORITY 2    ///< Ưu tiên task IR

#define MCP_TASK_STACK 4096   ///< Stack task MCP
#define MOTOR_TASK_STACK 2048 ///< Stack task motor
#define IR_TASK_STACK 2048    ///< Stack task IR

#define MOTOR_CMD_QUEUE_SIZE 5 ///< Số phần tử hàng đợi lệnh motor
#define IR_CMD_QUEUE_SIZE 5    ///< Số phần tử hàng đợi lệnh IR

#endif /* _CONFIG_H_ */
