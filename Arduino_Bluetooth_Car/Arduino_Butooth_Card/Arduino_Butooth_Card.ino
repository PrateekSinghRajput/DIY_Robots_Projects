char t;

void setup() {
  pinMode(12, OUTPUT);  // left motor forward
  pinMode(11, OUTPUT);  // left motor reverse
  pinMode(10, OUTPUT);  // right motor forward
  pinMode(9, OUTPUT);   // right motor reverse
  pinMode(8, OUTPUT);   // LED
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    t = Serial.read();
    Serial.println(t);
  }

  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);

  if (t == 'F') {
    digitalWrite(12, HIGH);
    digitalWrite(10, HIGH);
  } else if (t == 'G') {
    digitalWrite(11, HIGH);
    digitalWrite(9, HIGH);
  } else if (t == 'L') {
    digitalWrite(10, HIGH);
  } else if (t == 'R') {
    digitalWrite(12, HIGH);
  } else if (t == 'W') {
    digitalWrite(8, HIGH);
  } else if (t == 'w') {
    digitalWrite(8, LOW);
  } else if (t == 'S') {
  }

  delay(100);
}
