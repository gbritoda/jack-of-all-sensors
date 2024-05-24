#include "rfid.h"

byte* readCardUID() {
    byte uidBytes[10];
    for (byte i = 0; i < rfid_reader.uid.size; i++) {
        uidBytes[i] = rfid_reader.uid.uidByte[i];
    }
    return uidBytes;
}