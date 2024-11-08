#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h>

void setupLCD();
void updateLCD();
void displayMessage(const char* message, int duration = 2000);

#endif