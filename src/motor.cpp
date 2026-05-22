#include "motor.h"

/**
 * @brief Khởi tạo chân GPIO và PWM cho động cơ.
 */
void Motor_Init(void)
{
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);
	pinMode(ENA, OUTPUT);
	pinMode(ENB, OUTPUT);

	ledcSetup(CH1, 1000, 8);
	ledcAttachPin(ENA, CH1);

	ledcSetup(CH2, 1000, 8);
	ledcAttachPin(ENB, CH2);

	Motor_Stop();
}

/**
 * @brief Điều khiển xe đi thẳng bằng cách xoay cả 2 động cơ tiến lên.
 */
void Motor_Forward(void)
{
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);
	ledcWrite(CH1, MOTOR_SPEED_FULL);
	ledcWrite(CH2, MOTOR_SPEED_FULL);
}

/**
 * @brief Điều khiển xe đi lùi bằng cách xoay cả 2 động cơ lùi lại.
 */
void Motor_Backward(void)
{
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW);
	ledcWrite(CH1, MOTOR_SPEED_TURN);
	ledcWrite(CH2, MOTOR_SPEED_TURN);
}

/**
 * @brief Điều khiển xe rẽ trái bằng cách tắt động cơ trái và xoay động cơ phải.
 */
void Motor_Left(void)
{
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);
	ledcWrite(CH1, 0);
	ledcWrite(CH2, MOTOR_SPEED_TURN);
}

/**
 * @brief Điều khiển xe rẽ phải bằng cách xoay động cơ trái và tắt động cơ phải.
 */
void Motor_Right(void)
{
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
	ledcWrite(CH1, MOTOR_SPEED_TURN);
	ledcWrite(CH2, 0);
}

/**
 * @brief Dừng toàn bộ động cơ xe.
 */
void Motor_Stop(void)
{
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
	ledcWrite(CH1, 0);
	ledcWrite(CH2, 0);
}

/**
 * @brief Quay động cơ xoay hướng quạt sang trái (chiều tiến).
 */
void Motor_FanSwing_Left(void)
{
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	ledcWrite(CH1, MOTOR_SPEED_TURN);
}

/**
 * @brief Quay động cơ xoay hướng quạt sang phải (chiều lùi).
 */
void Motor_FanSwing_Right(void)
{
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	ledcWrite(CH1, MOTOR_SPEED_TURN);
}

/**
 * @brief Dừng động cơ xoay hướng quạt.
 */
void Motor_FanSwing_Stop(void)
{
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	ledcWrite(CH1, 0);
}
