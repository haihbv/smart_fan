#ifndef _MAIN_H_
#define _MAIN_H_

#include <ArduinoJson.h>
#include <IRremote.hpp>
#include <WebSocketMCP.h>
#include <WiFi.h>
#include <WiFiManager.h>

// ============================================================================
// IR CONFIG
// ============================================================================
#define IR_SEND_PIN 4           ///< Chân GPIO phát tín hiệu hồng ngoại
#define IR_FAN_POWER 0xE31CFF00 ///< Mã HEX hồng ngoại điều khiển Bật/Tắt quạt
#define IR_FAN_SPEED                                                           \
  0xE916FF00 ///< Mã HEX hồng ngoại điều chỉnh tốc độ quạt (1->8->1)

// ============================================================================
// MOTOR CONFIG
// ============================================================================
#define IN1 26 ///< Chân điều khiển hướng động cơ Trái (1)
#define IN2 27 ///< Chân điều khiển hướng động cơ Trái (2)
#define IN3 14 ///< Chân điều khiển hướng động cơ Phải (1)
#define IN4 12 ///< Chân điều khiển hướng động cơ Phải (2)
#define ENA 25 ///< Chân PWM điều khiển tốc độ động cơ Trái
#define ENB 33 ///< Chân PWM điều khiển tốc độ động cơ Phải
#define CH1 0  ///< Kênh LEDC cho chân ENA
#define CH2 1  ///< Kênh LEDC cho chân ENB

#define MOTOR_SPEED_FULL 255 ///< Tốc độ tối đa khi đi thẳng/lùi
#define MOTOR_SPEED_TURN 220 ///< Tốc độ khi rẽ trái/phải

#define MOVE_DURATION_MS 5000 ///< Thời gian di chuyển thẳng/lùi mặc định (ms)
#define TURN_DURATION_MS 2000 ///< Thời gian rẽ trái/phải mặc định (ms)

/**
 * @brief Cấu trúc quản lý trạng thái di chuyển của xe.
 */
struct MotorState {
  bool active = false;          ///< Xe đang di chuyển hay không (đúng/sai)
  unsigned long startMs = 0;    ///< Thời điểm bắt đầu di chuyển (ms)
  unsigned long durationMs = 0; ///< Thời gian duy trì di chuyển (ms)
  String pendingAction =
      ""; ///< Hành động đang thực hiện (tiến, lùi, trái, phải)
};

extern MotorState motorState; ///< Thực thể quản lý trạng thái xe toàn cục

/**
 * @brief Đăng ký các công cụ điều khiển (MCP Tools) lên hệ thống.
 */
void registerMcpTools(void);

/**
 * @brief Gửi tín hiệu hồng ngoại bật/tắt quạt.
 */
void sendFanPowerToggle(void);

/**
 * @brief Gửi tín hiệu hồng ngoại chuyển sang tốc độ tiếp theo của quạt.
 */
void sendFanNextSpeed(void);

/**
 * @brief Điều khiển xe di chuyển thẳng về phía trước.
 */
void forward(void);

/**
 * @brief Điều khiển xe di chuyển lùi về phía sau.
 */
void backward(void);

/**
 * @brief Điều khiển xe rẽ trái (quay bánh phải).
 */
void left(void);

/**
 * @brief Điều khiển xe rẽ phải (quay bánh trái).
 */
void right(void);

/**
 * @brief Dừng toàn bộ động cơ xe.
 */
void stopCar(void);

#endif /* _MAIN_H_ */