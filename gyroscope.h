#ifndef _GYROSCOPE_H_
#define _GYROSCOPE_H_

#include <Wire.h>
#include <GY521.h>

// I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
#define GYROSCOPE_MPU 0x68

GY521 gyroscope(GYROSCOPE_MPU);

void setupGyro();
#endif