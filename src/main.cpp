#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ir.h"
#include "motor.h"
#include "task.h"

/**
 * @brief Hàm setup của Arduino, khởi tạo phần cứng và hệ thống FreeRTOS.
 */
void setup()
{
	Serial.begin(115200);

	Serial.println();
	Serial.println("=================================");
	Serial.println("ESP32 MCP CAR + FAN START");
	Serial.println("=================================");

	Ir_Init();
	Serial.println("[IR] Initialized");

	Motor_Init();
	Serial.println("[MOTOR] Initialized");

	Tasks_Init();
}

/**
 * @brief Vòng lặp chính, nhường CPU cho các task FreeRTOS.
 */
void loop()
{
	vTaskDelay(pdMS_TO_TICKS(1000));
}
