#include "ultrasonic_sensor.h"

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


int averageDistanceFromSonar(int n_measurements) {
    long totalDistance = 0;
    for (int i = 0; i < n_measurements; i++) {
        totalDistance += measureDistanceFromSonar();
        delay(5);
    }
    return totalDistance / n_measurements;
}