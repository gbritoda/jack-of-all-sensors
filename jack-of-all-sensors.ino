#include <SPI.h>

#include "ultrasonic_sensor.h"
#include "rfid.h"
#include "lcd_screen.h"
#include "tft_screen.h"

#define RGB0_R_PIN 6
#define RGB0_G_PIN 5
#define RGB0_B_PIN 4

#define VRX_PIN A0
#define VRY_PIN A1
#define JOY_SW_PIN 9

#define DELAY_BETWEEN_INPUTS_MS 300

#define SONAR_MAX_DIST 1500

// SPI pins, more for my sanity (values for Arduino Mega 2560 Rev3)
#define SPI_CS   53 // SS
#define SPI_SCLK 52 // SCK, SCLK
#define SPI_MOSI 51 // COPI, SDA
#define SPI_MISO 50 // CIPO


enum ScreenContext {
    CTX_HOME_SCREEN,
    CTX_SONAR_MODE,
    CTX_RFID_MODE,
};

struct {
    unsigned int redVal;
    unsigned int greenVal;
    unsigned int blueVal;
} rgb0;


ScreenContext context;
char *menuOptions[] = {"Sonar","RFID"};
const int numOpts = 2;
volatile int currentMenuSelection = 0;


void setRgb0Colour(int redValue, int greenValue, int blueValue) {
    analogWrite(RGB0_R_PIN, redValue);
    analogWrite(RGB0_G_PIN, greenValue);
    analogWrite(RGB0_B_PIN, blueValue);
    rgb0.redVal = redValue;
    rgb0.greenVal = greenValue;
    rgb0.blueVal = blueValue;
}


void displayHomeSelectionMenu(int selectedOpt = 0) {
    tftScreen.background(0,0,0);
    for (int i=0; i < numOpts; i++) {
        int h = (TFT_DEFAULT_CHAR_H*(i+1)); //char height * selection
        if (i == selectedOpt) {
            char selected_str[15] = ">";
            strcat(selected_str, menuOptions[i]);
            tftScreen.text(selected_str, 0, h);
        } else {
            char unselected_str[15] = " ";
            strcat(unselected_str, menuOptions[i]);
            tftScreen.text(unselected_str, 0, h);
        }
    }
}


void enterSelectedMode(int selectedOpt) {
    switch(selectedOpt) {
        case 0:
            runSonarMode();
            break;
        case 1:
            runRFIDMode();
            break;
    }
}


void runSonarMode() {
    context = CTX_SONAR_MODE;
    lcdScreen.clear();
    writeToLcd("Sonar Mode", 0, 0);
    clearTFTScreen();
    tftScreen.text("Sonar Mode",0,0);
    tftScreen.circle(tftScreen.width()*8/9, TFT_DEFAULT_CHAR_H/2, TFT_DEFAULT_CHAR_H/2);
    tftScreen.line(tftScreen.width()*8/9, TFT_DEFAULT_CHAR_H/2, (tftScreen.width()*8/9)+sqrt(2),0);

    int distance = averageDistanceFromSonar(10);
    int lastDistance = -1;
    while (true) {
        distance = averageDistanceFromSonar(10);
        if (distance >= SONAR_MAX_DIST) {
            // Red
            setRgb0Colour(100, 0, 0);
            clearTFTScreenBelow(0, tftScreen.height()/2);
            tftScreen.stroke(TFT_RED);
            tftScreen.text("Out of Reach!", 0, tftScreen.height()/2);
        } else {
            // Green
            setRgb0Colour(0, 100, 0);
            if (distance != lastDistance) {
                // clearTFTScreen();
                // Clear below the area the text is being written
                clearTFTScreenBelow(0, tftScreen.height()/2);
                tftScreen.setTextSize(TFT_CHAR_MULT+1);
                tftScreen.stroke(TFT_GREEN);
                char dist_str[16];
                itoa(distance, dist_str, 10);
                strcat(dist_str, " cm");
                tftScreen.text(dist_str, 0, tftScreen.height()/2);
                setDefaultTFTScheme();
            }
            lastDistance = distance;
        }
    }
}


void runRFIDMode() {
    context = CTX_RFID_MODE;
    lcdScreen.clear();
    lcdScreen.setCursor(0,0);
    bool rfidModeRead = true;

    while (true) {

        if (rfidModeRead == true) {
            RFIDModeReadUID();
        } else {
            RFIDModeWriteUid();
        }

        if (joySwitchPressed() == true) {
            rfidModeRead = !rfidModeRead;
        }

        delay(100);
    }
}



void RFIDModeReadUID() {
    lcdScreen.clear();
    writeToLcd("RFID UID:", 0,0);
    if (rfid_reader.PICC_IsNewCardPresent()) {
        setRgb0Colour(100, 75, 0);
        clearLcdRow(1);
        rfid_reader.PICC_ReadCardSerial();

        // card UID
        byte* uidBytes = readCardUID();
        for (byte i=0; i<rfid_reader.uid.size; i++) {
            lcdScreen.print(uidBytes[i], HEX);
        }

        while (rfid_reader.PICC_IsNewCardPresent()) {
            // Do nothing and wait for the card to be removed
            delay(100);  // Small delay to avoid overwhelming the loop
        }
        rfid_reader.PICC_HaltA();
        rfid_reader.PCD_StopCrypto1();

    } else {
        setRgb0Colour(0, 100, 0);
        clearLcdRow(1);
        lcdScreen.print("Nothing detected");
    }
    delay(100);
}


void RFIDModeWriteUid() {
    lcdScreen.clear();
    writeToLcd("Write new UID:",0,0);
    writeToLcd("MAINTENANCE",0,1);
    // setRgb0Colour(100, 100, 0);
    // // Set new UID
    // byte newUid[] = {0xDE, 0xAD, 0xBE, 0xEF};
    // if ( rfid_reader.MIFARE_SetUid(newUid, (byte)4, true) ) {
    //     writeToLcd("Written.", 0, 0);
    //     setRgb0Colour(0, 100, 0);
    // }
    
    // // Halt PICC and re-select it so DumpToSerial doesn't get confused
    // rfid_reader.PICC_HaltA();
    // if ( ! rfid_reader.PICC_IsNewCardPresent() || ! rfid_reader.PICC_ReadCardSerial() ) {
    //     return;
    // }
}


// Triggers a press on the rising edge (Joystick switch unpressed is HIGH)
bool joySwitchPressed() {
    if (digitalRead(JOY_SW_PIN) == LOW) {
        delay(50);
        // Wait for High
        // FIXME: Possibly a timeout?
        while (digitalRead(JOY_SW_PIN) != HIGH) {
            delay(50);
        }
        return true;
    }
    return false;
}


void setup() {
    Serial.begin(115200); // For debug purposes
    // Setup Home screen
    context = CTX_HOME_SCREEN;

    // RFID Reader
    SPI.begin();
    rfid_reader.PCD_Init();

    // Clear ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Defining RGB0
    pinMode(RGB0_R_PIN, OUTPUT);
    pinMode(RGB0_G_PIN, OUTPUT);
    pinMode(RGB0_B_PIN, OUTPUT);

    // JoyStick button is pressed when input is LOW
    pinMode(JOY_SW_PIN, INPUT_PULLUP);

    setRgb0Colour(0, 0, 100);

    // Initialise LCD
    lcdScreen.begin(16, 2);

    // Init TFT screen
    tftScreen.begin();
    setDefaultTFTScheme();
    clearTFTScreen();
    displayHomeSelectionMenu(currentMenuSelection);
}


void loop() {
    context = CTX_HOME_SCREEN;
    writeToLcd("Welcome 2 Jack ", 0, 0);
    writeToLcd("of All Sensors!", 0, 1);
    int xValue = analogRead(VRX_PIN);
    int yValue = analogRead(VRY_PIN);

    if (yValue >= 900 && currentMenuSelection < numOpts-1) {
        currentMenuSelection++;
        delay(DELAY_BETWEEN_INPUTS_MS);
        displayHomeSelectionMenu(currentMenuSelection);
    } else if (yValue <= 100 && currentMenuSelection > 0) {
        currentMenuSelection--;
        delay(DELAY_BETWEEN_INPUTS_MS);
        displayHomeSelectionMenu(currentMenuSelection);
    }

    if (joySwitchPressed() == true) {
        enterSelectedMode(currentMenuSelection);
    }

    delay(100);
}

