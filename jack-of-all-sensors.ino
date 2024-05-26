#include <SPI.h>

#include "ultrasonic_sensor.h"
#include "rfid.h"
#include "lcd.h"

#define RGB0_R_PIN 6
#define RGB0_G_PIN 5
#define RGB0_B_PIN 4

#define VRX_PIN A0
#define VRY_PIN A1
#define JOY_SW_PIN 9

#define DELAY_BETWEEN_INPUTS_MS 300

#define SONAR_MAX_DIST 1500

enum ScreenContext {
    CTX_HOME_SCREEN,
    CTX_SONAR_MODE,
    CTX_RFID_MODE,
};

struct {
    unsigned int n;
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
    lcd.clear();
    for (int i=0; i < numOpts; i++) {
        if (i == selectedOpt) {
            lcd.setCursor(0,0);
            lcd.print("> ");
            lcd.print(menuOptions[i]);
            
            if (i+1 < numOpts) {
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(menuOptions[i+1]);
            }
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
    writeToLcd("Distance (cm):", 0, 0);
    clearLcdRow(1);
    context = CTX_SONAR_MODE;

    while (true) {
        int distance = averageDistanceFromSonar();
        clearLcdRow(1);
        if (distance >= SONAR_MAX_DIST) {
            // Red
            setRgb0Colour(100, 0, 0);
            lcd.print("Out of reach");
        } else {
            // Green
            setRgb0Colour(0, 100, 0);
            lcd.print("    ");
            lcd.print(distance);
            lcd.print("cm");
            lcd.print("    ");
        }
    }
}


void runRFIDMode() {
    context = CTX_RFID_MODE;
    lcd.clear();
    lcd.setCursor(0,0);
    bool rfidModeRead = true;

    while (true) {

        if (rfidModeRead == true) {
            RFIDModeReadUID();
        } else {
            RFIDModeWriteUid();
        }

        if (joySwitchPressed() == true) {
            rfidModeRead = !rfidModeRead;
            delay(200);
        }

        delay(100);
    }
}



void RFIDModeReadUID() {
    lcd.clear();
    writeToLcd("RFID UID:", 0,0);
    if (rfid_reader.PICC_IsNewCardPresent()) {
        setRgb0Colour(100, 75, 0);
        clearLcdRow(1);
        rfid_reader.PICC_ReadCardSerial();

        // card UID
        byte* uidBytes = readCardUID();
        for (byte i=0; i<rfid_reader.uid.size; i++) {
            lcd.print(uidBytes[i], HEX);
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
        lcd.print("Nothing detected");
    }
    delay(100);
}


void RFIDModeWriteUid() {
    lcd.clear();
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
    rgb0.n = 0;
    pinMode(RGB0_R_PIN, OUTPUT);
    pinMode(RGB0_G_PIN, OUTPUT);
    pinMode(RGB0_B_PIN, OUTPUT);

    // JoyStick button is pressed when input is LOW
    pinMode(JOY_SW_PIN, INPUT_PULLUP);

    setRgb0Colour(0, 0, 100);
    // Initialise LCD
    lcd.begin(16, 2);
}


void loop() {
    context = CTX_HOME_SCREEN;
    int xValue = analogRead(VRX_PIN);
    int yValue = analogRead(VRY_PIN);

    if (yValue >= 900 && currentMenuSelection < numOpts-1) {
        currentMenuSelection++;
        delay(DELAY_BETWEEN_INPUTS_MS);
    } else if (yValue <= 100 && currentMenuSelection > 0) {
        currentMenuSelection--;
        delay(DELAY_BETWEEN_INPUTS_MS);
    }

    displayHomeSelectionMenu(currentMenuSelection);
    if (joySwitchPressed() == true) {
        enterSelectedMode(currentMenuSelection);
    }

    delay(100);
}

