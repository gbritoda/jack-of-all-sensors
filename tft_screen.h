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

TFT tftScreen = TFT(TFT_CS, TFT_A0, TFT_RST);

void setDefaultTFTScheme();

void clearTFTScreen();