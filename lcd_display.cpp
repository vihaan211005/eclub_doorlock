#include "lcd_display.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setupLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void displayMessage(const char* message, int duration) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(duration);
}