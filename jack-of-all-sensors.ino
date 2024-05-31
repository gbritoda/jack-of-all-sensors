#include <SPI.h>

#include "ultrasonic_sensor.h"
#include "rfid.h"
#include "tft_screen.h"
#include "gyroscope.h"
#include "temp_sensor.h"

#define SILENT_MODE

#define RGB0_R_PIN 6
#define RGB0_G_PIN 5
#define RGB0_B_PIN 4

#define RGB_YELLOW 150, 100, 0
#define RGB_BLUE 0, 0, 100
#define RGB_RED 100, 0, 0
#define RGB_GREEN 0, 100, 0

#define VRX_PIN A0
#define VRY_PIN A1
#define JOY_SW_PIN 9

#define RETURN_BUTTON_PIN 22

#define BUZZER_PIN 23

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

char *menuOptions[] = {"Sonar","RFID","Gyro", "Temp&Humidity"};
const int numOpts = 4;
volatile int currentMenuSelection = 0;


void setRgb0Colour(int redValue, int greenValue, int blueValue) {
    analogWrite(RGB0_R_PIN, redValue);
    analogWrite(RGB0_G_PIN, greenValue);
    analogWrite(RGB0_B_PIN, blueValue);
    rgb0.redVal = redValue;
    rgb0.greenVal = greenValue;
    rgb0.blueVal = blueValue;
}


void enterSelectedMode(int selectedOpt) {
    switch(selectedOpt) {
        case 0:
            runSonarMode();
            break;
        case 1:
            runRFIDMode();
            break;
        case 2:
            runGyroscopeMode();
            break;
        case 3:
            runTempHumidityMode();
            break;
    }
}


void runSonarMode() {
    context = CTX_SONAR_MODE;
    clearTFTScreen();
    tftScreen.text("Sonar Mode",0,0);
    tftScreen.circle(tftScreen.width()*8/9, TFT_DEFAULT_CHAR_H/2, TFT_DEFAULT_CHAR_H/2);
    tftScreen.line(tftScreen.width()*8/9, TFT_DEFAULT_CHAR_H/2, (tftScreen.width()*8/9)+sqrt(2),0);

    int distance = averageDistanceFromSonar(10);
    int lastDistance = -1;
    while (!returnButtonRisingEdge()) {
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
    playReturnBeep();
}

void runRFIDMode() {
    context = CTX_RFID_MODE;
    clearTFTScreen();
    bool readModeCardDetected = false;
    bool refreshScreen = true;
    setRgb0Colour(150, 100, 0);
    while (!returnButtonRisingEdge()) {
        readModeCardDetected = RFIDModeReadUID(readModeCardDetected, refreshScreen);
        refreshScreen = false;
        delay(200);
    }
    playReturnBeep();
}

void runGyroscopeMode() {
    setupGyro();
    while (!returnButtonRisingEdge()) {
        clearTFTScreen();
        gyroscope.read();
        tftScreen.println("Acceleration");
        tftScreen.println(gyroscope.getAccelX());
        tftScreen.println(gyroscope.getAccelY());
        tftScreen.println(gyroscope.getAccelZ());
        tftScreen.println("Angle");
        tftScreen.println(gyroscope.getAngleX());
        tftScreen.println(gyroscope.getAngleY());
        tftScreen.println(gyroscope.getAngleZ());
        delay(200);
    }
    playReturnBeep();

}

void runTempHumidityMode() {
    while (!returnButtonRisingEdge()) {
        clearTFTScreen();
        dht11_sensor.read11(DHT11_PIN);
        tftScreen.println("Humidity: ");
        tftScreen.print(dht11_sensor.humidity);
        tftScreen.println("%");
        
        tftScreen.println("Temp:");
        tftScreen.print(dht11_sensor.temperature);
        tftScreen.println("degC");
        delay(200);
    }
    playReturnBeep();
}


/* Returns true if there was a card, False if not. Also takes a parameter of last state 
so we don't have to constantly print out the message in case it doesn't change state. */
bool RFIDModeReadUID(bool cardDetectedPreviously, bool refreshScreen) {
    if (rfidReader.PICC_IsNewCardPresent()) {
        // TODO: Next step for this would be to pause and wait for the card to be removed
        // This way there's no flickering
        setRgb0Colour(RGB_GREEN);
        
        rfidReader.PICC_ReadCardSerial();

        // card UID
        byte uidBytes[10];
        int uidBytesSize = rfidReader.uid.size;
        readCardUID(uidBytes, uidBytesSize);
        displayRFIDReadMode(true, uidBytes, uidBytesSize, refreshScreen);

        while (rfidReader.PICC_IsNewCardPresent() || rfidReader.PICC_ReadCardSerial()) {
            // Do nothing and wait for the card to be removed
            delay(300);  // Small delay to avoid overwhelming the loop
        }
        rfidReader.PICC_HaltA();
        rfidReader.PCD_StopCrypto1();
        return true;
    } else {
        if (cardDetectedPreviously) {
            setRgb0Colour(RGB_YELLOW);
            displayRFIDReadMode(false, nullptr, 0, refreshScreen);
        }
        return false;
    }
}

// Triggers a press on the rising edge (Joystick switch unpressed is HIGH)

bool detectRisingEdge(int pin) {
    if (digitalRead(pin) == LOW) {
        delay(50);
        // Wait for High
        // FIXME: Possibly a timeout?
        while (digitalRead(pin) != HIGH) {
            delay(50);
        }
        return true;
    }
    return false;
}

bool joySwitchRisingEdge() {
    return detectRisingEdge(JOY_SW_PIN);
}

bool returnButtonRisingEdge() {
    return detectRisingEdge(RETURN_BUTTON_PIN);
}

/**
 * @brief Plays a sound in the buzzer
 * 
 * @param f Frequency of the tone
 * @param duration Time to play the tone in miliseconds
 */
void playSound(int f, int duration) {
    #ifndef SILENT_MODE
    tone(BUZZER_PIN, f);
    delay(duration);
    noTone(BUZZER_PIN);
    #endif
}

void playFalloutTheme() {
    // Intro theme C D# E
    playSound(523, 1000);
    playSound(622, 2000);
    playSound(659, 2000);
}

void playConfirmationBeep() {
    playSound(700, 200);
}

void playReturnBeep() {
    playSound(500, 200);
}

void setup() {
    Serial.begin(115200); // For debug purposes
    // Setup Home screen
    context = CTX_HOME_SCREEN;

    // RFID Reader
    SPI.begin();
    rfidReader.PCD_Init();

    // Ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Return button -> Unpressed = HIGH
    pinMode(RETURN_BUTTON_PIN, INPUT);

    // Buzzer
    pinMode(BUZZER_PIN, OUTPUT);

    // Defining RGB0
    pinMode(RGB0_R_PIN, OUTPUT);
    pinMode(RGB0_G_PIN, OUTPUT);
    pinMode(RGB0_B_PIN, OUTPUT);

    // JoyStick button is pressed when input is LOW
    pinMode(JOY_SW_PIN, INPUT_PULLUP);

    pinMode(DHT11_PIN, INPUT);

    setRgb0Colour(RGB_GREEN);

    // Init TFT screen
    tftScreen.begin();
    setDefaultTFTScheme();
    clearTFTScreen();
    // Why not. Adds an extra 5 seconds to the boot up though, will change it to a simple beep when I get bored of it
    playFalloutTheme();
    tftScreen.println("    Jack of");
    tftScreen.println(" All Sensors");
    displayHomeSelectionMenu(currentMenuSelection, menuOptions, numOpts);
}


void loop() {
    context = CTX_HOME_SCREEN;
    int xValue = analogRead(VRX_PIN);
    int yValue = analogRead(VRY_PIN);

    if (yValue >= 900 && currentMenuSelection < numOpts-1) {
        currentMenuSelection++;
        delay(DELAY_BETWEEN_INPUTS_MS);
        displayHomeSelectionMenu(currentMenuSelection, menuOptions, numOpts);
    } else if (yValue <= 100 && currentMenuSelection > 0) {
        currentMenuSelection--;
        delay(DELAY_BETWEEN_INPUTS_MS);
        displayHomeSelectionMenu(currentMenuSelection, menuOptions, numOpts);
    }

    if (joySwitchRisingEdge() == true) {
        playSound(700, 200);
        enterSelectedMode(currentMenuSelection);
        // If this function returns we get back to the main screen
        setRgb0Colour(RGB_GREEN);
        clearTFTScreen();
        tftScreen.println("    Jack of");
        tftScreen.println(" All Sensors");
        displayHomeSelectionMenu(currentMenuSelection, menuOptions, numOpts);
    }

    delay(100);
}

