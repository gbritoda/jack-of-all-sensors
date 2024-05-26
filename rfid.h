#pragma once

#include <MFRC522.h>

#define RFID_SS_PIN 53
#define RFID_RST_PIN 13

MFRC522 rfid_reader(RFID_SS_PIN, RFID_RST_PIN);

byte* readCardUID();