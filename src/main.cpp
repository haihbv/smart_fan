#include "main.h"

const char *mcpEndpoint =
    "wss://api.xiaozhi.me/mcp/"
    "?token=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9."
    "eyJ1c2VySWQiOjkzMzM5OCwiYWdlbnRJZCI6MTg0OTYzNiwiZW5kcG9pbnRJZCI6ImFnZW50Xz"
    "E4NDk2MzYiLCJwdXJwb3NlIjoibWNwLWVuZHBvaW50IiwiaWF0IjoxNzc5MTcxMDMzLCJleHAi"
    "OjE4MTA3Mjg2MzN9.cVNymqwvIwCBSmppU65Z3nJ38Y_C-"
    "YNcgIpbBgqzpOiJSFRwtYvtbOnq3b3PmiFtVuT7ez9vHXPVpz-b5LctPA"; ///< Địa chỉ
                                                                 ///< máy chủ
                                                                 ///< WebSocket
                                                                 ///< MCP

WebSocketMCP mcpClient; ///< Đối tượng điều khiển giao tiếp WebSocket MCP

bool fanPowerOn =
    false; ///< Trạng thái nguồn quạt hồng ngoại (đúng: đang bật, sai: đang tắt)

bool mcpToolsRegistered =
    false; ///< Đánh dấu trạng thái đăng ký công cụ MCP (đúng: đã đăng ký)

/**
 * @brief Thực thể toàn cục quản lý trạng thái di chuyển của xe.
 */
MotorState motorState;

/**
 * @brief Hàm xử lý khi trạng thái kết nối tới máy chủ MCP thay đổi.
 * @param connected Trạng thái kết nối hiện tại (true nếu đã kết nối thành công)
 */
void onConnectionStatus(bool connected) {
  if (connected) {
    Serial.println("[MCP] Connected");
    registerMcpTools();
  } else {
    Serial.println("[MCP] Disconnected");
    mcpToolsRegistered = false;
    stopCar();
  }
}

/**
 * @brief Khởi tạo và đăng ký các công cụ điều khiển quạt và xe với máy chủ MCP.
 */
void registerMcpTools() {
  if (mcpToolsRegistered) {
    Serial.println("[MCP] Tools already registered");
    return;
  }

  // ==================================================
  // FAN TOOL
  // ==================================================

  mcpClient.registerTool(
      "fan_control",
      "Dieu khien quat hong ngoai bang IR. "
      "power_on de bat quat. "
      "power_off de tat quat. "
      "next_speed de chuyen sang muc quat tiep theo.",
      "{\"type\":\"object\","
      "\"properties\":{"
      "\"device\":{\"type\":\"string\",\"enum\":[\"fan\"]},"
      "\"action\":{\"type\":\"string\",\"enum\":["
      "\"power_on\","
      "\"power_off\","
      "\"next_speed\"]}"
      "},"
      "\"required\":[\"device\",\"action\"]}",
      [](const String &args) -> WebSocketMCP::ToolResponse {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, args);

        if (error) {
          return WebSocketMCP::ToolResponse(
              "{\"success\":false,\"error\":\"invalid_json\"}");
        }

        String device = doc["device"].as<String>();
        String action = doc["action"].as<String>();
        bool irSent = false;

        if (device != "fan") {
          return WebSocketMCP::ToolResponse(
              "{\"success\":false,\"error\":\"unsupported_device\"}");
        }

        if (action == "power_on") {
          if (!fanPowerOn) {
            sendFanPowerToggle();
            fanPowerOn = true;
            irSent = true;
            Serial.println("[FAN] POWER ON");
          }
        } else if (action == "power_off") {
          if (fanPowerOn) {
            sendFanPowerToggle();
            fanPowerOn = false;
            irSent = true;
            Serial.println("[FAN] POWER OFF");
          }
        } else if (action == "next_speed") {
          if (fanPowerOn) {
            sendFanNextSpeed();
            irSent = true;
            Serial.println("[FAN] NEXT SPEED");
          }
        } else {
          return WebSocketMCP::ToolResponse(
              "{\"success\":false,\"error\":\"unsupported_action\"}");
        }

        return WebSocketMCP::ToolResponse("{\"success\":true,"
                                          "\"device\":\"fan\","
                                          "\"action\":\"" +
                                          action +
                                          "\","
                                          "\"ir_sent\":" +
                                          String(irSent ? "true" : "false") +
                                          ","
                                          "\"power\":\"" +
                                          String(fanPowerOn ? "on" : "off") +
                                          "\"}");
      });

  Serial.println("[MCP] Fan tool registered");

  // ==================================================
  // MOTOR TOOL
  // ==================================================

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
      [](const String &args) -> WebSocketMCP::ToolResponse {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, args);

        if (error) {
          return WebSocketMCP::ToolResponse(
              "{\"success\":false,\"error\":\"invalid_json\"}");
        }

        String action = doc["action"].as<String>();
        stopCar();

        if (action == "forward") {
          forward();
          motorState.active = true;
          motorState.startMs = millis();
          motorState.durationMs = MOVE_DURATION_MS;
          motorState.pendingAction = action;
          Serial.println("[CAR] FORWARD");
        } else if (action == "backward") {
          backward();
          motorState.active = true;
          motorState.startMs = millis();
          motorState.durationMs = MOVE_DURATION_MS;
          motorState.pendingAction = action;
          Serial.println("[CAR] BACKWARD");
        } else if (action == "left") {
          left();
          motorState.active = true;
          motorState.startMs = millis();
          motorState.durationMs = TURN_DURATION_MS;
          motorState.pendingAction = action;
          Serial.println("[CAR] LEFT");
        } else if (action == "right") {
          right();
          motorState.active = true;
          motorState.startMs = millis();
          motorState.durationMs = TURN_DURATION_MS;
          motorState.pendingAction = action;
          Serial.println("[CAR] RIGHT");
        } else if (action == "stop") {
          stopCar();
          motorState.active = false;
          motorState.pendingAction = "";
          Serial.println("[CAR] STOP");
        } else {
          return WebSocketMCP::ToolResponse(
              "{\"success\":false,\"error\":\"unsupported_action\"}");
        }

        return WebSocketMCP::ToolResponse("{\"success\":true,\"action\":\"" +
                                          action + "\"}");
      });

  Serial.println("[MCP] Car tool registered");
  mcpToolsRegistered = true;
}

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32 MCP CAR + FAN START");
  Serial.println("=================================");

  IrSender.begin(IR_SEND_PIN);
  Serial.println("[IR] Initialized");

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

  stopCar();
  Serial.println("[CAR] Motor initialized");

  // WiFiManager sẽ lưu SSID và mật khẩu mà bạn đã cấu hình vào bộ nhớ trong
  // (NVS) của ESP32. Ví dụ, nếu trước đó bạn đã kết nối với WiFi có tên là
  // "ESP32" thì nó sẽ tự động nhớ và kết nối lại mỗi khi khởi động
  WiFiManager wifiManager;

  if (!wifiManager.autoConnect("SMART FAN", "66668888")) {
    Serial.println("[WIFI] Failed");
    delay(3000);
    ESP.restart();
  }

  Serial.println("[WIFI] Connected");
  Serial.println("[WIFI] IP: " + WiFi.localIP().toString());
  Serial.println("=================================");

  mcpClient.begin(mcpEndpoint, onConnectionStatus);
}

void loop() {
  // kiểm tra & duy trì kết nối WiFi
  static unsigned long lastReconnect = 0;

  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastReconnect > 5000) {
      Serial.println("[WIFI] Reconnecting...");
      WiFi.reconnect();
      lastReconnect = millis();
    }
    stopCar();
    mcpClient.loop();
    delay(10);
    return;
  }

  // khi động cơ di chuyển quá thời gian đã định thì dừng động cơ
  if (motorState.active &&
      (millis() - motorState.startMs >= motorState.durationMs)) {
    stopCar();
    motorState.active = false;
    Serial.println("[CAR] STOP after " + motorState.pendingAction);
  }

  // duy trì hoạt động MCP
  mcpClient.loop();
  delay(10);
}

/**
 * @brief Gửi mã hồng ngoại để bật/tắt nguồn quạt.
 */
void sendFanPowerToggle() {
  Serial.println("[IR] POWER TOGGLE");
  IrSender.sendNECRaw(IR_FAN_POWER, 0);
  delay(80);
}

/**
 * @brief Gửi mã hồng ngoại để chuyển sang cấp tốc độ quạt kế tiếp.
 */
void sendFanNextSpeed() {
  Serial.println("[IR] NEXT SPEED");
  IrSender.sendNECRaw(IR_FAN_SPEED, 0);
  delay(80);
}

/**
 * @brief Điều khiển xe đi thẳng bằng cách xoay cả 2 động cơ tiến lên với tốc độ
 * tối đa.
 */
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(CH1, MOTOR_SPEED_FULL);
  ledcWrite(CH2, MOTOR_SPEED_FULL);
}

/**
 * @brief Điều khiển xe đi lùi bằng cách xoay cả 2 động cơ ngược lại với tốc độ
 * rẽ.
 */
void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(CH1, MOTOR_SPEED_TURN);
  ledcWrite(CH2, MOTOR_SPEED_TURN);
}

/**
 * @brief Điều khiển xe rẽ sang trái bằng cách tắt động cơ trái và xoay động cơ
 * phải tiến lên.
 */
void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(CH1, 0);
  ledcWrite(CH2, MOTOR_SPEED_TURN);
}

/**
 * @brief Điều khiển xe rẽ sang phải bằng cách xoay động cơ trái tiến lên và tắt
 * động cơ phải.
 */
void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(CH1, MOTOR_SPEED_TURN);
  ledcWrite(CH2, 0);
}

/**
 * @brief Dừng xe lập tức bằng cách hạ các chân điều khiển xuống mức thấp và đặt
 * tốc độ PWM về 0.
 */
void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(CH1, 0);
  ledcWrite(CH2, 0);
}
