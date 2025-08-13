#include <esp_now.h>
#include <WiFi.h>

// Motor control pins
#define in1 26
#define in2 27
#define in3 14
#define in4 12
#define spd 32

// PWM parameters
const int freq = 30000;
const int pwmChannel = 2;
const int resolution = 8;

// Define structure for received data
typedef struct struct_message {
  int front;
  int back;
  int left;
  int right;
  int speed;
} struct_message;

struct_message data;

// Callback for received data
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  // Set motor speed
  ledcWrite(pwmChannel, data.speed);

  // Motor direction control
  if (data.back == 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else if (data.front == 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else if (data.right == 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else if (data.left == 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); 

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(spd, OUTPUT);

  // Initialize PWM
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(spd, pwmChannel);
}

void loop() {
  
}
