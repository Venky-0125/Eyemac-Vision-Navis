// EYEMAC VISION NAVIS - Tinkercad simulation sketch (Arduino UNO)
// HC-SR04 distance -> buzzer + vibration motor alert (Report, Appendix I)
#define TRIG 9
#define ECHO 8
#define BUZZER 3
#define MOTOR 7
#define BUTTON 2

long duration;
float distance;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  // Trigger the ultrasonic pulse
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = (duration * 0.0343) / 2.0; // distance in cm

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Obstacle within 20 cm -> alert
  if (distance > 0 && distance <= 20) {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(MOTOR, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
    digitalWrite(MOTOR, LOW);
  }
  delay(200);
}
