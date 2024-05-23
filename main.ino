// sudo chmod a+rw /dev/ttyACM0
#include <LiquidCrystal.h>


#define TRIG_PIN 2
#define ECHO_PIN 3

#define RGB0_R_PIN 6
#define RGB0_G_PIN 5
#define RGB0_B_PIN 4

#define VRX_PIN A0
#define VRY_PIN A1

#define SONAR_MAX_DIST 1500

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

void setup() {
    // Serial.begin(9600); // For debug purposes
    // Clear ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Defining the RGB0 pins as OUTPUT
    pinMode(RGB0_R_PIN, OUTPUT);
    pinMode(RGB0_G_PIN, OUTPUT);
    pinMode(RGB0_B_PIN, OUTPUT);

    // define 
    setRgb0Colour(0, 0, 0);
    // Initialise LCD
    lcd.begin(16, 2);
    lcd.clear();
}

void runSonarMode() {
    // This will be hidden away for menu options later
    writeToLcd("Distance (Sonar):");
    clearLcdRow(1);

    // loop here
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

void loop() {

    int xValue = analogRead(VRX_PIN);
    int yValue = analogRead(VRY_PIN);


    delay(100);
}

