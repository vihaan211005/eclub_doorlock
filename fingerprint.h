#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <Adafruit_Fingerprint.h>

extern Adafruit_Fingerprint finger;

uint8_t getFingerprintEnroll(uint8_t id, String name, String password, String mailiD);
void getFingerprintID();
void fingerCheck();

#endif