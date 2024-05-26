#include "lcd_screen.h"

void clearLcdRow(int row) {
    lcdScreen.setCursor(0,row);
    lcdScreen.print("                ");
    lcdScreen.setCursor(0,row);
}

void writeToLcd(const String& text, int column, int row) {
    lcdScreen.setCursor(column, row);
    lcdScreen.print(text);
}