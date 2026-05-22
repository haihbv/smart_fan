#ifndef _TASK_H_
#define _TASK_H_

#include <Arduino.h>

/**
 * @brief Enum lệnh điều khiển động cơ.
 */
enum MotorCmd
{
	MOTOR_CMD_FORWARD = 0,
	MOTOR_CMD_BACKWARD,
	MOTOR_CMD_LEFT,
	MOTOR_CMD_RIGHT,
	MOTOR_CMD_STOP,
	MOTOR_CMD_FAN_SWING_AUTO
};

/**
 * @brief Enum lệnh điều khiển hồng ngoại.
 */
enum IrCmd
{
	IR_CMD_POWER_TOGGLE = 0,
	IR_CMD_NEXT_SPEED
};

/**
 * @brief Khởi tạo hệ thống FreeRTOS và MCP.
 */
void Tasks_Init(void);

#endif /* _TASK_H_ */
