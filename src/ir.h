#ifndef _IR_H_
#define _IR_H_

#include <Arduino.h>
#include "config.h"

/**
 * @brief Khởi tạo phần cứng IR.
 */
void Ir_Init(void);

/**
 * @brief Gửi mã hồng ngoại để bật/tắt nguồn quạt.
 */
void Ir_SendFanPowerToggle(void);

/**
 * @brief Gửi mã hồng ngoại để chuyển sang cấp tốc độ quạt kế tiếp.
 */
void Ir_SendFanNextSpeed(void);

#endif /* _IR_H_ */
