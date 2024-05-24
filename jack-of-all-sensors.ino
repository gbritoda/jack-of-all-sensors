// sudo chmod a+rw /dev/ttyACM0
#include <LiquidCrystal.h>


#define TRIG_PIN 2
#define ECHO_PIN 3

#define RGB0_R_PIN 6
#define RGB0_G_PIN 5
#define RGB0_B_PIN 4

#define VRX_PIN A0
#define VRY_PIN A1
#define JOY_SW_PIN 53

#define DELAY_BETWEEN_INPUTS_MS 300

#define SONAR_MAX_DIST 1500

char *menuOptions[] = {"Sonar","NFC", "Radio", "Other"};
const int numOpts = 4;
int currentMenuSelection = 0;

LiquidCrystal lcd(22,23,24,25,26,27,28,29,30,31,32);


void setRgb0Colour(int redValue, int greenValue, int blueValue) {
    analogWrite(RGB0_R_PIN, redValue);
    analogWrite(RGB0_G_PIN, greenValue);
    analogWrite(RGB0_B_PIN, blueValue);
}


void clearLcdRow(int row = 0) {
    lcd.setCursor(0,row);
    lcd.print("                ");
    lcd.setCursor(0,row);
}


void writeToLcd(const String& text, int column=0, int row=0) {
    lcd.setCursor(column, row);
    lcd.print(text);
}


int measureDistanceFromSonar() {
    long duration;
    // Clear pin
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    // pulseIn returns time in microseconds
    duration = pulseIn(ECHO_PIN, HIGH);
    // distance = time (us) * sound speed (cm/us) / 2
    return duration*0.03043/2;
}

int averageDistanceFromSonar(int n_measurements = 5) {
    long totalDistance = 0;
    for (int i = 0; i < n_measurements; i++) {
        totalDistance += measureDistanceFromSonar();
        delay(5);
    }
    return totalDistance / n_measurements;
}


void runSonarMode() {
    writeToLcd("Distance (Sonar):");
    clearLcdRow(1);

    // FIXME: add an interrupt
    while (true) {
        int distance = averageDistanceFromSonar();
        clearLcdRow(1);
        if (distance >= SONAR_MAX_DIST) {
            // Red
            setRgb0Colour(255, 0, 0);
            lcd.print("Out of reach");
        } else {
            // Green
            setRgb0Colour(0, 255, 0);
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
        // default:
        //     goHome();
    }
}


void setup() {
    // Serial.begin(115200); // For debug purposes
    // Clear ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Defining the RGB0 pins as OUTPUT
    pinMode(RGB0_R_PIN, OUTPUT);
    pinMode(RGB0_G_PIN, OUTPUT);
    pinMode(RGB0_B_PIN, OUTPUT);

    // JoyStick button is pressed when input is LOW
    pinMode(JOY_SW_PIN, INPUT_PULLUP);

    setRgb0Colour(0, 0, 0);
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

