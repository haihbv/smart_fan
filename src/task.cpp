#include "task.h"

#include <ArduinoJson.h>
#include <WebSocketMCP.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "config.h"
#include "ir.h"
#include "motor.h"

/**
 * @brief Đối tượng điều khiển giao tiếp WebSocket MCP.
 */
static WebSocketMCP mcpClient;

/**
 * @brief Hàng đợi lệnh điều khiển động cơ.
 */
static QueueHandle_t motorCmdQueue = nullptr;

/**
 * @brief Hàng đợi lệnh điều khiển hồng ngoại.
 */
static QueueHandle_t irCmdQueue = nullptr;

/**
 * @brief Handle của các task để theo dõi stack.
 */
static TaskHandle_t mcpTaskHandle = nullptr;
static TaskHandle_t motorTaskHandle = nullptr;
static TaskHandle_t irTaskHandle = nullptr;

/**
 * @brief Trạng thái đăng ký công cụ MCP.
 */
static bool mcpToolsRegistered = false;

/**
 * @brief Trạng thái kết nối MCP hiện tại.
 */
static bool mcpConnected = false;

/**
 * @brief Thời điểm cuối MCP còn kết nối tốt.
 */
static TickType_t lastMcpOkTick = 0;

/**
 * @brief Trạng thái nguồn quạt hồng ngoại.
 */
static bool fanPowerOn = false;

/**
 * @brief Hàm đăng ký tool MCP.
 */
static void RegisterMcpTools(void);

/**
 * @brief Task xử lý MCP và WiFi watchdog.
 */
static void McpTask(void *param);

/**
 * @brief Task xử lý điều khiển động cơ.
 */
static void MotorTask(void *param);

/**
 * @brief Task xử lý điều khiển hồng ngoại.
 */
static void IrTask(void *param);

/**
 * @brief Đẩy lệnh motor vào hàng đợi.
 */
static bool QueueMotorCmd(MotorCmd cmd)
{
	if (motorCmdQueue == nullptr)
	{
		return false;
	}

	if (cmd == MOTOR_CMD_STOP)
	{
		xQueueReset(motorCmdQueue);
	}

	return xQueueSend(motorCmdQueue, &cmd, 0) == pdTRUE;
}

/**
 * @brief Đẩy lệnh IR vào hàng đợi.
 */
static bool QueueIrCmd(IrCmd cmd)
{
	return (irCmdQueue != nullptr) &&
		   (xQueueSend(irCmdQueue, &cmd, 0) == pdTRUE);
}

/**
 * @brief Callback khi trạng thái kết nối MCP thay đổi.
 */
static void OnConnectionStatus(bool connected)
{
	mcpConnected = connected;

	if (connected)
	{
		lastMcpOkTick = xTaskGetTickCount();
		RegisterMcpTools();
		Serial.println("[MCP] Connected");
	}
	else
	{
		mcpToolsRegistered = false;
		Motor_Stop();
		Motor_FanSwing_Stop();
		Serial.println("[MCP] Disconnected");
	}
}

/**
 * @brief Đăng ký các tool điều khiển quạt và xe với MCP.
 */
static void RegisterMcpTools(void)
{
	if (mcpToolsRegistered)
	{
		Serial.println("[MCP] Tools already registered");
		return;
	}

	mcpClient.registerTool(
		"fan_control",
		"Dieu khien quat hong ngoai bang IR. "
		"power_on de bat quat. "
		"power_off de tat quat. "
		"next_speed de chuyen sang muc quat tiep theo. "
		"swing_auto de bat xoay tu dong.",
		"{\"type\":\"object\","
		"\"properties\":{"
		"\"device\":{\"type\":\"string\",\"enum\":[\"fan\"]},"
		"\"action\":{\"type\":\"string\",\"enum\":["
		"\"power_on\","
		"\"power_off\","
		"\"next_speed\","
		"\"swing_auto\"]}"
		"},"
		"\"required\":[\"device\",\"action\"]}",
		[](const String &args) -> WebSocketMCP::ToolResponse
		{
			JsonDocument doc;
			DeserializationError error = deserializeJson(doc, args);

			if (error)
			{
				return WebSocketMCP::ToolResponse(
					"{\"success\":false,\"error\":\"invalid_"
					"json\"}");
			}

			String device = doc["device"].as<String>();
			String action = doc["action"].as<String>();
			bool shouldQueue = false;
			bool queued = false;

			if (device != "fan")
			{
				return WebSocketMCP::ToolResponse(
					"{\"success\":false,\"error\":\"unsupported_"
					"device\"}");
			}

			if (action == "power_on")
			{
				if (!fanPowerOn)
				{
					shouldQueue = true;
					queued = QueueIrCmd(IR_CMD_POWER_TOGGLE);
					if (queued)
					{
						fanPowerOn = true;
						Serial.println("[FAN] POWER ON");
					}
				}
			}
			else if (action == "power_off")
			{
				if (fanPowerOn)
				{
					shouldQueue = true;
					queued = QueueIrCmd(IR_CMD_POWER_TOGGLE);
					if (queued)
					{
						fanPowerOn = false;
						Serial.println("[FAN] POWER OFF");
					}
				}
			}
			else if (action == "next_speed")
			{
				if (fanPowerOn)
				{
					shouldQueue = true;
					queued = QueueIrCmd(IR_CMD_NEXT_SPEED);
					if (queued)
					{
						Serial.println("[FAN] NEXT SPEED");
					}
				}
			}
			else if (action == "swing_auto")
			{
				shouldQueue = true;
				queued = QueueMotorCmd(MOTOR_CMD_FAN_SWING_AUTO);
				if (queued)
				{
					Serial.println("[FAN] SWING AUTO");
				}
			}
			else
			{
				return WebSocketMCP::ToolResponse(
					"{\"success\":false,\"error\":\"unsupported_"
					"action\"}");
			}

			if (shouldQueue && !queued)
			{
				return WebSocketMCP::ToolResponse(
					"{\"success\":false,\"error\":\"queue_"
					"full\"}");
			}

			return WebSocketMCP::ToolResponse(
				"{\"success\":true,"
				"\"device\":\"fan\","
				"\"action\":\"" +
				action +
				"\","
				"\"queued\":" +
				String(queued ? "true" : "false") +
				","
				"\"power\":\"" +
				String(fanPowerOn ? "on" : "off") + "\"}");
		});

	Serial.println("[MCP] Fan tool registered");

	mcpClient.registerTool(
		"car_control",
		"Dieu khien xe bang dong co. "
		"forward de di thang. "
		"backward de di lui. "
		"left de quay trai. "
		"right de quay phai. "
		"stop de dung xe.",
		"{\"type\":\"object\","
		"\"properties\":{"
		"\"action\":{\"type\":\"string\",\"enum\":["
		"\"forward\","
		"\"backward\","
		"\"left\","
		"\"right\","
		"\"stop\"]}"
		"},"
		"\"required\":[\"action\"]}",
		[](const String &args) -> WebSocketMCP::ToolResponse
		{
			JsonDocument doc;
			DeserializationError error = deserializeJson(doc, args);

			if (error)
			{
				return WebSocketMCP::ToolResponse(
					"{\"success\":false,\"error\":\"invalid_"
					"json\"}");
			}

			String action = doc["action"].as<String>();
			MotorCmd cmd = MOTOR_CMD_STOP;

			if (action == "forward")
			{
				cmd = MOTOR_CMD_FORWARD;
			}
			else if (action == "backward")
			{
				cmd = MOTOR_CMD_BACKWARD;
			}
			else if (action == "left")
			{
				cmd = MOTOR_CMD_LEFT;
			}
			else if (action == "right")
			{
				cmd = MOTOR_CMD_RIGHT;
			}
			else if (action == "stop")
			{
				cmd = MOTOR_CMD_STOP;
			}
			else
			{
				return WebSocketMCP::ToolResponse(
					"{\"success\":false,\"error\":\"unsupported_"
					"action\"}");
			}

			if (!QueueMotorCmd(cmd))
			{
				return WebSocketMCP::ToolResponse(
					"{\"success\":false,\"error\":\"queue_"
					"full\"}");
			}

			return WebSocketMCP::ToolResponse(
				"{\"success\":true,\"action\":\"" + action +
				"\",\"queued\":true}");
		});

	Serial.println("[MCP] Car tool registered");
	mcpToolsRegistered = true;
}

/**
 * @brief Khởi tạo hệ thống FreeRTOS và MCP.
 */
void Tasks_Init(void)
{
	WiFiManager wifiManager;

	if (!wifiManager.autoConnect(WIFI_AP_SSID, WIFI_AP_PASS))
	{
		Serial.println("[WIFI] Failed");
		delay(3000);
		ESP.restart();
	}

	Serial.println("[WIFI] Connected");
	Serial.println("[WIFI] IP: " + WiFi.localIP().toString());
	Serial.println("=================================");

	motorCmdQueue = xQueueCreate(MOTOR_CMD_QUEUE_SIZE, sizeof(MotorCmd));
	irCmdQueue = xQueueCreate(IR_CMD_QUEUE_SIZE, sizeof(IrCmd));

	if (motorCmdQueue == nullptr || irCmdQueue == nullptr)
	{
		Serial.println("[QUEUE] Create failed");
		delay(1000);
		ESP.restart();
	}

	mcpClient.begin(MCP_ENDPOINT, OnConnectionStatus);
	lastMcpOkTick = xTaskGetTickCount();

	xTaskCreatePinnedToCore(McpTask, "mcpTask", MCP_TASK_STACK, nullptr,
							MCP_TASK_PRIORITY, &mcpTaskHandle,
							MCP_TASK_CORE);
	xTaskCreatePinnedToCore(MotorTask, "motorTask", MOTOR_TASK_STACK,
							nullptr, MOTOR_TASK_PRIORITY, &motorTaskHandle,
							MOTOR_TASK_CORE);
	xTaskCreatePinnedToCore(IrTask, "irTask", IR_TASK_STACK, nullptr,
							IR_TASK_PRIORITY, &irTaskHandle, IR_TASK_CORE);
}

/**
 * @brief Task xử lý MCP và WiFi watchdog.
 */
static void McpTask(void *param)
{
	TickType_t lastReconnectTick = 0;
	TickType_t lastStackLogTick = xTaskGetTickCount();
	const TickType_t loopDelay = pdMS_TO_TICKS(10);

	for (;;)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			TickType_t now = xTaskGetTickCount();
			if (now - lastReconnectTick >=
				pdMS_TO_TICKS(WIFI_RECONNECT_INTERVAL_MS))
			{
				Serial.println("[WIFI] Reconnecting...");
				WiFi.reconnect();
				lastReconnectTick = now;
			}
		}

		if (!mcpConnected)
		{
			TickType_t now = xTaskGetTickCount();
			if (now - lastMcpOkTick >=
				pdMS_TO_TICKS(MCP_WATCHDOG_TIMEOUT_MS))
			{
				Serial.println("[WDT] MCP timeout, restarting");
				ESP.restart();
			}
		}
		else
		{
			lastMcpOkTick = xTaskGetTickCount();
		}

		mcpClient.loop();

		if (xTaskGetTickCount() - lastStackLogTick >=
			pdMS_TO_TICKS(STACK_LOG_INTERVAL_MS))
		{
			Serial.printf("[STACK] mcp=%u motor=%u ir=%u\n",
						  (unsigned)uxTaskGetStackHighWaterMark(
							  mcpTaskHandle),
						  (unsigned)uxTaskGetStackHighWaterMark(
							  motorTaskHandle),
						  (unsigned)uxTaskGetStackHighWaterMark(
							  irTaskHandle));
			lastStackLogTick = xTaskGetTickCount();
		}

		vTaskDelay(loopDelay);
	}
}

/**
 * @brief Dừng toàn bộ động cơ trước khi chuyển trạng thái.
 */
static void MotorTask_StopAll(void)
{
	Motor_Stop();
	Motor_FanSwing_Stop();
}

/**
 * @brief Xử lý lệnh motor và thiết lập thời gian chạy.
 */
static void MotorTask_ApplyCmd(MotorCmd cmd, bool *active,
						   TickType_t *remainingTicks, bool *autoSwing,
						   int8_t *autoDirection)
{
	MotorTask_StopAll();
	*active = false;
	*remainingTicks = 0;
	*autoSwing = false;
	*autoDirection = FAN_SWING_CENTER;

	switch (cmd)
	{
	case MOTOR_CMD_FORWARD:
		Motor_Forward();
		*remainingTicks = pdMS_TO_TICKS(MOVE_DURATION_MS);
		*active = true;
		Serial.println("[CAR] FORWARD");
		break;
	case MOTOR_CMD_BACKWARD:
		Motor_Backward();
		*remainingTicks = pdMS_TO_TICKS(MOVE_DURATION_MS);
		*active = true;
		Serial.println("[CAR] BACKWARD");
		break;
	case MOTOR_CMD_LEFT:
		Motor_Left();
		*remainingTicks = pdMS_TO_TICKS(TURN_DURATION_MS);
		*active = true;
		Serial.println("[CAR] LEFT");
		break;
	case MOTOR_CMD_RIGHT:
		Motor_Right();
		*remainingTicks = pdMS_TO_TICKS(TURN_DURATION_MS);
		*active = true;
		Serial.println("[CAR] RIGHT");
		break;
	case MOTOR_CMD_STOP:
		Serial.println("[CAR] STOP");
		break;
	case MOTOR_CMD_FAN_SWING_AUTO:
		*autoSwing = true;
		*autoDirection = FAN_SWING_LEFT;
		*remainingTicks = pdMS_TO_TICKS(FAN_SWING_STEP_MS);
		*active = true;
		Motor_FanSwing_Left();
		Serial.println("[FAN] SWING AUTO");
		break;
	default:
		break;
	}
}

/**
 * @brief Task xử lý điều khiển động cơ.
 */
static void MotorTask(void *param)
{
	MotorCmd cmd = MOTOR_CMD_STOP;
	bool active = false;
	bool autoSwing = false;
	int8_t autoDirection = FAN_SWING_CENTER;
	TickType_t remainingTicks = 0;
	const TickType_t stepTicks = pdMS_TO_TICKS(MOTOR_STEP_CHECK_MS);

	for (;;)
	{
		if (!active)
		{
			if (xQueueReceive(motorCmdQueue, &cmd, portMAX_DELAY) ==
				pdTRUE)
			{
				MotorTask_ApplyCmd(cmd, &active,
							   &remainingTicks, &autoSwing,
							   &autoDirection);
			}
			continue;
		}

		if (xQueueReceive(motorCmdQueue, &cmd, 0) == pdTRUE)
		{
			MotorTask_ApplyCmd(cmd, &active, &remainingTicks,
						   &autoSwing, &autoDirection);
			continue;
		}

		if (remainingTicks <= stepTicks)
		{
			if (autoSwing)
			{
				autoDirection =
					(autoDirection == FAN_SWING_LEFT) ?
					FAN_SWING_RIGHT : FAN_SWING_LEFT;
				if (autoDirection == FAN_SWING_LEFT)
				{
					Motor_FanSwing_Left();
				}
				else
				{
					Motor_FanSwing_Right();
				}
				remainingTicks = pdMS_TO_TICKS(FAN_SWING_RETURN_MS);
				vTaskDelay(stepTicks);
				continue;
			}

			MotorTask_StopAll();
			active = false;
			remainingTicks = 0;
			continue;
		}

		remainingTicks -= stepTicks;
		vTaskDelay(stepTicks);
	}
}

/**
 * @brief Task xử lý điều khiển hồng ngoại.
 */
static void IrTask(void *param)
{
	IrCmd cmd = IR_CMD_POWER_TOGGLE;

	for (;;)
	{
		if (xQueueReceive(irCmdQueue, &cmd, portMAX_DELAY) == pdTRUE)
		{
			switch (cmd)
			{
			case IR_CMD_POWER_TOGGLE:
				Ir_SendFanPowerToggle();
				break;
			case IR_CMD_NEXT_SPEED:
				Ir_SendFanNextSpeed();
				break;
			default:
				break;
			}

			vTaskDelay(pdMS_TO_TICKS(80));
		}
	}
}
