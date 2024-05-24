#pragma once

#include <LiquidCrystal.h>


LiquidCrystal lcd(22,23,24,25,26,27,28,29,30,31,32);


void clearLcdRow(int row=0);

void writeToLcd(const String& text, int column=0, int row=0);