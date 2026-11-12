
#include <SoftPWM.h>
#include <SoftwareSerial.h>

// UART vom ESP32
// ESP32 GPIO17 -> Arduino A0
SoftwareSerial espSerial(A0, A1);

// SunFounder GalaxyRVR Motorpins
const int in1 = 2;
const int in2 = 3;
const int in3 = 4;
const int in4 = 5;

// Einstellungen
const int maxSpeed = 180;
const int lowerLimit = 1000;
const int upperLimit = 3000;

unsigned long lastMessage = 0;

// Motorsteuerung links
void setLeftMotor(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    SoftPWMSet(in1, speed);
    SoftPWMSet(in2, 0);
  }
  else if (speed < 0) {
    SoftPWMSet(in1, 0);
    SoftPWMSet(in2, -speed);
  }
  else {
    SoftPWMSet(in1, 0);
    SoftPWMSet(in2, 0);
  }
}

// Motorsteuerung rechts
void setRightMotor(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    SoftPWMSet(in3, 0);
    SoftPWMSet(in4, speed);
  }
  else if (speed < 0) {
    SoftPWMSet(in3, -speed);
    SoftPWMSet(in4, 0);
  }
  else {
    SoftPWMSet(in3, 0);
    SoftPWMSet(in4, 0);
  }
}

void stopMotors() {
  setLeftMotor(0);
  setRightMotor(0);
}

// Joystickwert in Motorgeschwindigkeit umwandeln
int joystickToSpeed(int value) {
  if (value < lowerLimit) {
    return map(value, 0, lowerLimit, -maxSpeed, 0);
  }
  else if (value > upperLimit) {
    return map(value, upperLimit, 4095, 0, maxSpeed);
  }
  else {
    return 0;
  }
}

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);

  SoftPWMBegin();

  SoftPWMSet(in1, 0);
  SoftPWMSet(in2, 0);
  SoftPWMSet(in3, 0);
  SoftPWMSet(in4, 0);

  stopMotors();

  Serial.println("GalaxyRVR bereit!");
}

void loop() {
  static char buffer[32];
  static byte index = 0;

  while (espSerial.available()) {
    char c = espSerial.read();

    if (c == '\n') {
      buffer[index] = '\0';
      index = 0;

      int x, y, button;

      if (sscanf(buffer, "%d,%d,%d",
                 &x, &y, &button) == 3 &&
          x >= 0 && x <= 4095 &&
          y >= 0 && y <= 4095 &&
          (button == 0 || button == 1)) {

        // Joystickachsen auswerten
        int steering = joystickToSpeed(x);
        int throttle = joystickToSpeed(y);

        // Differentialsteuerung
        int leftSpeed = constrain(
          throttle + steering,
          -maxSpeed, maxSpeed
        );

        int rightSpeed = constrain(
          throttle - steering,
          -maxSpeed, maxSpeed
        );

        // Button = Notstopp
        if (button == 0) {
          leftSpeed = 0;
          rightSpeed = 0;
        }

        setLeftMotor(leftSpeed);
        setRightMotor(rightSpeed);

        lastMessage = millis();

        // Debug
        Serial.print("X: ");
        Serial.print(x);
        Serial.print(" Y: ");
        Serial.print(y);

        Serial.print(" | Left: ");
        Serial.print(leftSpeed);

        Serial.print(" Right: ");
        Serial.println(rightSpeed);
      }
    }
    else if (c != '\r') {
      if (index < sizeof(buffer) - 1) {
        buffer[index++] = c;
      }
      else {
        index = 0;
      }
    }
  }

  // Sicherheitsstopp bei Verbindungsverlust
  if (millis() - lastMessage > 300) {
    stopMotors();
  }
}
