#include <SPI.h>

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

#include <Wire.h>

#include "servo_control.h"
#include "wifi_config.h"
#include "web_server.h"

using namespace std;



void setup() {
  Serial.begin(115200);
  setupServo();
  setupWiFi();
  setupWebServer();
}

void loop() {
  checkIPChange();
  serverHandle();
}
