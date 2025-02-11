#include "tft_screen.h"

void setDefaultTFTScheme() {
    tftScreen.stroke(TFT_GREEN);
    //  By default, characters are 5x8 pixels (wxh), here we set them to 10x16
    tftScreen.setTextSize(TFT_CHAR_MULT);
}

void clearTFTScreen() {
    tftScreen.background(0,0,0);
    tftScreen.setCursor(0, 0);
}

void clearTFTScreenBelow(uint16_t x, uint16_t y) {
    tftScreen.fillRect(x, y, tftScreen.width(), tftScreen.height(), 0);
    tftScreen.setCursor(x,y);
}


void displayHomeSelectionMenu(int selectedOpt, char* menuOptions[], int numOpts) {
    clearTFTScreenBelow(0, 2*TFT_DEFAULT_CHAR_H);
    setDefaultTFTScheme();
    for (int i=0; i < numOpts; i++) {
        int h = (TFT_DEFAULT_CHAR_H*(i+1)); //char height * selection
        if (i == selectedOpt) {
            char selected_str[15] = ">";
            strcat(selected_str, menuOptions[i]);
            tftScreen.println(selected_str);
        } else {
            char unselected_str[15] = " ";
            strcat(unselected_str, menuOptions[i]);
            tftScreen.println(unselected_str);
        }
    }
}


/* RFID Specific screens */
void displayRFIDReadMode(bool cardDetected, byte* uidBytes, int uidBytesSize, bool refreshScreen) {
    if (refreshScreen) {
        clearTFTScreen();
        tftScreen.println("RFID Read");
    }
    if (cardDetected) {
        clearTFTScreenBelow(0, 2*TFT_DEFAULT_CHAR_H);

        if (uidBytes != nullptr) {
            for (byte i=0; i<uidBytesSize; i++) {
                tftScreen.print(uidBytes[i], HEX);
            }
        } else {
            tftScreen.stroke(TFT_RED);
            tftScreen.println("Something went wrong!");
            setDefaultTFTScheme();
        }

    } else {
        clearTFTScreenBelow(0, 2*TFT_DEFAULT_CHAR_H);
        tftScreen.textSize(TFT_CHAR_MULT);
        tftScreen.stroke(TFT_YELLOW);
        tftScreen.setCursor(0,2*TFT_DEFAULT_CHAR_H);
        tftScreen.println("Standing by");
        setDefaultTFTScheme();
    }
}
