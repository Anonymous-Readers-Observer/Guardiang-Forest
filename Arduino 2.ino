int tiltPin = 13; // pin ke SW-2000

void setup() {
  pinMode(tiltPin, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int state = digitalRead(tiltPin);

  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);

  if (state == HIGH) {
    // Sensor tegak
    digitalWrite(10, HIGH); // LED hijau
    Serial.println("Tegak");
  } else {
    // Sensor miring
    digitalWrite(8, HIGH); // LED merah
    digitalWrite(9, HIGH); // LED kuning
    Serial.println("Miring");
  }

  delay(100);
}
