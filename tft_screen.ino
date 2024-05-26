#include "tft_screen.h"

void setDefaultTFTScheme() {
    tftScreen.stroke(0, 142, 0);
    //  By default, characters are 5x8 pixels (wxh), here we set them to 10x16
    tftScreen.setTextSize(TFT_CHAR_MULT);
}

void clearTFTScreen() {
    tftScreen.background(0, 0, 0);
}