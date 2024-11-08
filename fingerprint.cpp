#include "fingerprint.h"
#include "member_management.h"
#include "lcd_display.h"
#include "servo_control.h"
#include "email_sender.h"

HardwareSerial fingerSerial(2);  // Serial2 on ESP32 (TX=GPIO17, RX=GPIO16)
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

void setupFingerprint() {
  fingerSerial.begin(57600);  // Initialize HardwareSerial for the fingerprint sensor

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    ESP.restart();
    while (1) { delay(1); }
  }
  finger.getParameters();
}


uint8_t getFingerprintEnroll(uint8_t id, String name, String password, String mailiD) {
  int p = -1;
  Serial.print("Waiting for a valid finger to enroll as #");
  Serial.println(id);


  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        displayMessage("Image taken", 0);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        displayMessage("place finger", 0);
        break;

      default:
        Serial.println("Unknown error");
        displayMessage("error", 0);
        break;
    }
  }

  p = finger.image2Tz(1);


  Serial.println("Remove finger");

  displayMessage("remove finger", 2000);
  p = 0;

  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place the same finger again");

  displayMessage("place again", 2000);


  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        displayMessage("place finger", 0);
        break;
        // case FINGERPRINT_PACKETRECIEVEERR:

      default:
        Serial.println("Unknown error");
        break;
    }
  }

  p = finger.image2Tz(2);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");

    displayMessage("prints matched", 2000);
    saveMember(Member(name, id, false, password, mailiD));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    displayMessage("error", 0);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");

    displayMessage("did not match", 2000);
    return p;
  } else {
    Serial.println("Unknown error");

    displayMessage("error", 0);
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    displayMessage("stored", 0);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    displayMessage("error", 0);
    return p;
  }


  else {
    Serial.println("Unknown error");
    return p;
  }
  return true;
}

void getFingerprintID() {
  uint8_t p = finger.getImage();
  p = finger.image2Tz();
  p = finger.fingerSearch();

  if (p == FINGERPRINT_OK) {


    Member* foundMember = getMemberByID(finger.fingerID);


    Serial.print("Name: ");

    Serial.print(foundMember->name);
    Serial.print("  ID: ");
    Serial.print(finger.fingerID);
    Serial.print(" Confidence: ");
    Serial.println(finger.confidence);
    // displayMessage(foundMember.name + "  " + finger.fingerID , 0);

    if (foundMember->inside_status) {
      foundMember->inside_status = !foundMember->inside_status;
      // displayMessage("bye " + foundMember.name, 2000);
      return;
    } else {
      // displayMessage("hello " + foundMember.name, 0);
      foundMember->inside_status = !foundMember->inside_status;
      String content = foundMember->name + " has accessed the club through biometric authentication";
      sendMail((String)AUTHOR_EMAIL, "Club Opened", content);
      unlock();
      delay(2000);
      return;
    }

    // return finger.fingerID;
    return;
  }

  else {
    Serial.println("Did not find a match");
    displayMessage("not found", 2000);
    // return p;
    return;
  }
}

void fingerCheck() {
  if (finger.getImage() == !FINGERPRINT_NOFINGER) {
    getFingerprintID();
  }
}