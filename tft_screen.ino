#include "tft_screen.h"

void setDefaultTFTScheme() {
    tftScreen.stroke(TFT_GREEN);
    //  By default, characters are 5x8 pixels (wxh), here we set them to 10x16
    tftScreen.setTextSize(TFT_CHAR_MULT);
}

void clearTFTScreen() {
    tftScreen.background(0,0,0);
}

void clearTFTScreenBelow(uint16_t x, uint16_t y) {
    tftScreen.fillRect(x, y, tftScreen.width(), tftScreen.height(), 0);
}