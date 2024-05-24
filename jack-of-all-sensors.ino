#include <SPI.h>

#include "ultrasonic_sensor.h"
#include "rfid.h"
#include "lcd.h"

#define RGB0_R_PIN 6
#define RGB0_G_PIN 5
#define RGB0_B_PIN 4

#define VRX_PIN A0
#define VRY_PIN A1
#define JOY_SW_PIN 34

#define DELAY_BETWEEN_INPUTS_MS 300

#define SONAR_MAX_DIST 1500


char *menuOptions[] = {"Sonar","RFID"};
const int numOpts = 2;
int currentMenuSelection = 0;


void setRgb0Colour(int redValue, int greenValue, int blueValue) {
    analogWrite(RGB0_R_PIN, redValue);
    analogWrite(RGB0_G_PIN, greenValue);
    analogWrite(RGB0_B_PIN, blueValue);
}


void runSonarMode() {
    writeToLcd("Distance (cm):", 0, 0);
    clearLcdRow(1);

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

void displaySelectionMenu(int selectedOpt = 0) {
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

void goHome() {
    currentMenuSelection = 0;
    displaySelectionMenu();
}

void enterSelectedMode() {
    switch(currentMenuSelection) {
        case 0:
            runSonarMode();
            break;
        case 1:
            runRFIDMode();
            break;
        // default:
        //     goHome();
    }
}

void runRFIDMode() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("RFID");

    while (true) {
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
        delay(300);
    }
}

void setup() {
    Serial.begin(9600); // For debug purposes
    
    // RFID Reader
    SPI.begin();
    rfid_reader.PCD_Init();

    // Clear ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Defining the RGB0 pins as OUTPUT
    pinMode(RGB0_R_PIN, OUTPUT);
    pinMode(RGB0_G_PIN, OUTPUT);
    pinMode(RGB0_B_PIN, OUTPUT);

    // JoyStick button is pressed when input is LOW
    pinMode(JOY_SW_PIN, INPUT_PULLUP);

    setRgb0Colour(0, 0, 100);
    // Initialise LCD
    lcd.begin(16, 2);
    displaySelectionMenu();
}


void loop() {
    int xValue = analogRead(VRX_PIN);
    int yValue = analogRead(VRY_PIN);

    if (yValue >= 900 && currentMenuSelection < numOpts-1) {
        currentMenuSelection++;
        delay(DELAY_BETWEEN_INPUTS_MS);
    } else if (yValue <= 100 && currentMenuSelection > 0) {
        currentMenuSelection--;
        delay(DELAY_BETWEEN_INPUTS_MS);
    }

    displaySelectionMenu(currentMenuSelection);
    
    if (digitalRead(JOY_SW_PIN) == LOW) {
        enterSelectedMode();
    }


    delay(100);
}

