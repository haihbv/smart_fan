#include "config.h"
#include <IRremote.hpp>
#include "ir.h"

/**
 * @brief Khởi tạo phần cứng IR.
 */
void Ir_Init(void)
{
	IrSender.begin(IR_SEND_PIN);
}

/**
 * @brief Gửi mã hồng ngoại để bật/tắt nguồn quạt.
 */
void Ir_SendFanPowerToggle(void)
{
	Serial.println("[IR] POWER TOGGLE");
	IrSender.sendNECRaw(IR_FAN_POWER, 0);
}

/**
 * @brief Gửi mã hồng ngoại để chuyển sang cấp tốc độ quạt kế tiếp.
 */
void Ir_SendFanNextSpeed(void)
{
	Serial.println("[IR] NEXT SPEED");
	IrSender.sendNECRaw(IR_FAN_SPEED, 0);
}
