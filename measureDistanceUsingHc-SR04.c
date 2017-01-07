void firePulse(int triggerPin) {
  // Hold the trigger pin high for 10 us
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
}

int measurePulseWidthIncm(int echoPin) {
  unsigned long t1;
  unsigned long t2;
  unsigned long pulse_width;
  int cm;

  // Wait for pulse on echo pin
  while (digitalRead(echoPin) == 0);

  // Measure how long the echo pin was held high (pulse width)
  t1 = micros();
  while ( digitalRead(echoPin) == 1);
  t2 = micros();
  pulse_width = t2 - t1;

  // Convert to centimeters, using the speed of sound.
  cm = pulse_width / 58.0;
  if (cm > 100 || cm < 0) { return 100; }  // Only reliable to 100cm
  return cm;
}

int measureDistanceWithUltrasonicSensor(int triggerPinNumber, int echoPinNumber) {
  firePulse(triggerPinNumber);
  return measurePulseWidthIncm(echoPinNumber);
}


void setup() {
  Serial.begin(9600);
  pinMode(0, OUTPUT);   // Distance Sensor Trigger
  digitalWrite(0, LOW);    // Distance Sensor Trigger
}

void loop() {
  Serial.println(measureDistanceWithUltrasonicSensor(0, 0));
}