
#include <AFMotor.h>

AF_DCMotor motor(2);  // Motor an M1

void setup() {
  Serial.begin(115200);
  motor.setSpeed(120);
}

void loop() {
  Serial.println("FORWARD");
  motor.run(FORWARD);
  delay(3000);

  Serial.println("STOP");
  motor.run(RELEASE);
  delay(1000);

  Serial.println("BACKWARD");
  motor.run(BACKWARD);
  delay(3000);

  Serial.println("STOP");
  motor.run(RELEASE);
  delay(1000);
}
