
#include <WiFi.h>
#include <esp_now.h>

#define JOY_X 34
#define JOY_Y 35
#define JOY_SW 32

typedef struct {
  int x;
  int y;
  int button;
} JoystickData;

JoystickData data;

// MAC-Adresse des Empfängers eintragen
uint8_t receiverMAC[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

void setup() {
  Serial.begin(115200);

  pinMode(JOY_SW, INPUT_PULLUP);
  analogReadResolution(12);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Fehler!");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer Fehler!");
    return;
  }
}

void loop() {
  data.x = analogRead(JOY_X);
  data.y = analogRead(JOY_Y);

  // Button gedrückt = 0 = Notstopp
  data.button = digitalRead(JOY_SW);

  esp_now_send(
    receiverMAC,
    (uint8_t *)&data,
    sizeof(data)
  );

  Serial.printf("X: %d Y: %d Button: %d\n",
                data.x, data.y, data.button);

  delay(50);
}
