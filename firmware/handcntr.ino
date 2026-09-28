#include <Wire.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Adafruit_PWMServoDriver.h>
#include "config.h"

// =============================
// WiFi
// =============================

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// =============================
// PCA9685
// =============================

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVO_MIN 200
#define SERVO_MAX 500

// =============================
// WebSocket
// =============================

WebSocketsServer webSocket(81);

// =============================
// Servo configuration
// =============================

// Current positions
float currentAngle[5] = {
  90,
  130,
  130,
  90,
  90
};

// Target positions
float targetAngle[5] = {
  90,
  130,
  130,
  90,
  90
};

// Joint limits
float minAngle[5] = {
  10,   // CH0
  30,   // CH1
  30,   // CH2
  10,   // CH3
  10    // CH4
};

float maxAngle[5] = {
  170,  // CH0
  170,  // CH1
  170,  // CH2
  170,  // CH3
  170   // CH4
};

// Speed for each joint (degrees/second)
float speed[5] = {
  15,
  5,
  5,
  15,
  15
};

unsigned long lastUpdate = 0;


// =============================
// Write servo
// =============================

void writeServo(uint8_t channel, float angle)
{
  angle = constrain(angle, 0, 180);

  int pulse = map(
    (int)angle,
    0,
    180,
    SERVO_MIN,
    SERVO_MAX
  );

  pwm.setPWM(channel, 0, pulse);
}


// =============================
// Smooth servo movement
// =============================

void updateServos()
{
  unsigned long now = millis();

  float dt = (now - lastUpdate) / 1000.0;

  if (dt <= 0)
    return;

  lastUpdate = now;

  for (int i = 0; i < 5; i++)
  {
    float difference = targetAngle[i] - currentAngle[i];

    float movement = speed[i] * dt;

    if (abs(difference) <= movement)
    {
      currentAngle[i] = targetAngle[i];
    }
    else if (difference > 0)
    {
      currentAngle[i] += movement;
    }
    else
    {
      currentAngle[i] -= movement;
    }

    writeServo(i, currentAngle[i]);
  }
}


// =============================
// WebSocket event
// =============================

void webSocketEvent(
  uint8_t client,
  WStype_t type,
  uint8_t* payload,
  size_t length
)
{
  if (type != WStype_TEXT)
    return;

  StaticJsonDocument<200> doc;

  DeserializationError error =
    deserializeJson(doc, payload);

  if (error)
  {
    Serial.println("Invalid JSON");
    return;
  }

  // -----------------------------
  // Joint command
  // -----------------------------

  if (doc["type"] == "joint")
  {
    int channel = doc["ch"];
    float angle = doc["angle"];

    if (channel >= 0 && channel < 5)
    {
      angle = constrain(
        angle,
        minAngle[channel],
        maxAngle[channel]
      );

      targetAngle[channel] = angle;

      Serial.print("CH");
      Serial.print(channel);
      Serial.print(" target: ");
      Serial.println(angle);
    }
  }

  // -----------------------------
  // HOME
  // -----------------------------

  else if (doc["type"] == "home")
  {
    targetAngle[0] = 90;
    targetAngle[1] = 130;
    targetAngle[2] = 130;
    targetAngle[3] = 90;
    targetAngle[4] = 90;

    Serial.println("HOME");
  }

  // -----------------------------
  // STOP
  // -----------------------------

  else if (doc["type"] == "stop")
  {
    for (int i = 0; i < 5; i++)
    {
      targetAngle[i] = currentAngle[i];
    }

    Serial.println("STOP");
  }
}


// =============================
// Setup
// =============================

void setup()
{
  Serial.begin(115200);

  // I2C
  Wire.begin(21, 22);

  // PCA9685
  pwm.begin();
  pwm.setPWMFreq(50);

  delay(500);

  // Initial position
  for (int i = 0; i < 5; i++)
  {
    writeServo(i, currentAngle[i]);
  }

  // WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("WebSocket server running on port 81");

  lastUpdate = millis();
}


// =============================
// Main loop
// =============================

void loop()
{
  webSocket.loop();

  updateServos();
}