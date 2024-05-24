#include "lcd.h"

void clearLcdRow(int row) {
    lcd.setCursor(0,row);
    lcd.print("                ");
    lcd.setCursor(0,row);
}

void writeToLcd(const String& text, int column, int row) {
    lcd.setCursor(column, row);
    lcd.print(text);
}