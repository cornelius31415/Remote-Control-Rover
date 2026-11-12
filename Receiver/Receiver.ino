
#include <WiFi.h>
#include <esp_now.h>

// UART zum Arduino
#define UART_RX 16
#define UART_TX 17

// Muss identisch mit dem Sender sein
typedef struct {
  int x;
  int y;
  int button;
} JoystickData;

JoystickData receivedData = {2048, 2048, 0};

unsigned long lastReceived = 0;

// ESP-NOW Empfang (Arduino ESP32 Core 3.x)
void onDataRecv(
  const esp_now_recv_info_t *info,
  const uint8_t *incomingData,
  int len
) {
  if (len != sizeof(JoystickData)) return;

  JoystickData temp;
  memcpy(&temp, incomingData, sizeof(temp));

  if (temp.x < 0 || temp.x > 4095 ||
      temp.y < 0 || temp.y > 4095 ||
      (temp.button != 0 && temp.button != 1)) {
    return;
  }

  receivedData = temp;
  lastReceived = millis();
}

void setup() {
  Serial.begin(115200);

  // ESP32 GPIO17 TX -> Arduino A0 RX
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  WiFi.mode(WIFI_STA);

  Serial.print("Empfaenger MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Fehler!");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);

  Serial.println("Empfaenger bereit!");
}

void loop() {
  JoystickData data = receivedData;

  // Funkverlust -> Notstopp
  if (millis() - lastReceived > 200) {
    data.x = 2048;
    data.y = 2048;
    data.button = 0;
  }

  // Genau das Format, das dein Arduino erwartet
  Serial2.printf("%d,%d,%d\n",
                 data.x,
                 data.y,
                 data.button);

  // Debug
  Serial.printf("X: %d Y: %d Button: %d\n",
                data.x, data.y, data.button);

  delay(50);
}
