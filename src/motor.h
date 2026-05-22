#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <Arduino.h>
#include "config.h"

/**
 * @brief Khởi tạo phần cứng điều khiển động cơ.
 */
void Motor_Init(void);

/**
 * @brief Điều khiển xe đi thẳng bằng cách xoay cả 2 động cơ tiến lên.
 */
void Motor_Forward(void);

/**
 * @brief Điều khiển xe đi lùi bằng cách xoay cả 2 động cơ lùi lại.
 */
void Motor_Backward(void);

/**
 * @brief Điều khiển xe rẽ trái bằng cách tắt động cơ trái và xoay động cơ phải.
 */
void Motor_Left(void);

/**
 * @brief Điều khiển xe rẽ phải bằng cách xoay động cơ trái và tắt động cơ phải.
 */
void Motor_Right(void);

/**
 * @brief Dừng toàn bộ động cơ xe.
 */
void Motor_Stop(void);

/**
 * @brief Quay động cơ xoay hướng quạt sang trái (chiều tiến).
 */
void Motor_FanSwing_Left(void);

/**
 * @brief Quay động cơ xoay hướng quạt sang phải (chiều lùi).
 */
void Motor_FanSwing_Right(void);

/**
 * @brief Dừng động cơ xoay hướng quạt.
 */
void Motor_FanSwing_Stop(void);

#endif /* _MOTOR_H_ */
