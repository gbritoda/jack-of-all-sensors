#include "gyroscope.h"

void setupGyro() {
    Wire.begin();
    gyroscope.begin();
    gyroscope.setAccelSensitivity(2);
    gyroscope.setGyroSensitivity(1);
    gyroscope.setThrottle();
}