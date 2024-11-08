#include <SPI.h>

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

#include <Wire.h>

#include "servo_control.h"
#include "lcd_display.h"
#include "wifi_config.h"
#include "email_sender.h"
#include "member_management.h"
#include "fingerprint.h"
#include "web_server.h"

using namespace std;



void setup() {
  Serial.begin(115200);
  setupServo();
  setupLCD();
  setupWiFi();
  setupEmailSender();
  setupWebServer();
  loadDataFromCSVFile();
  //   if (!SD.begin()) {
  //       Serial.println("SD card initialization failed!");
  //       return;
  //   }

  // members.clear();
  // loadDataFromCSVFile(filename, members);

  // sendMail("dhruvm22@iitk.ac.in","IP",ip);
}

void loop() {
  checkIPChange();
  fingerCheck();
  serverHandle();
  displayMessage(" Welcome to ECLUB", 0);
  

  // finger.emptyDatabase();
  // members.clear();

  // saveToCSVFile(filename, members);
}


