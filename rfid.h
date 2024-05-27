#pragma once

#include <MFRC522.h>

#define RFID_SS_PIN 53
#define RFID_RST_PIN 13

MFRC522 rfidReader(RFID_SS_PIN, RFID_RST_PIN);

void readCardUID(byte *uidBytes, int &uidSize);