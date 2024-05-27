#pragma once
#include <TFT.h>
#include <SPI.h>

#define TFT_CS 12
#define TFT_A0 11
#define TFT_RST 10

#define TFT_CHAR_H 8
#define TFT_CHAR_W 5

#define TFT_CHAR_MULT 2
#define TFT_DEFAULT_CHAR_H TFT_CHAR_MULT*TFT_CHAR_H
#define TFT_DEFAULT_CHAR_W TFT_CHAR_MULT*TFT_CHAR_W

// For TFT the colours are in BGR due to internal wiring
// Maybe I should change these variables to be BGR_COLOUR
#define TFT_GREEN 0, 142, 0
#define TFT_RED 0, 0, 142
#define TFT_YELLOW 0, 142, 142

TFT tftScreen = TFT(TFT_CS, TFT_A0, TFT_RST);

void setDefaultTFTScheme();

void clearTFTScreen();

void clearTFTScreenBelow(uint16_t x, uint16_t y);


/* RFID specific screens */
void displayRFIDReadMode(bool cardDetected, byte* uidBytes, int uidBytesSize, bool refreshScreen);