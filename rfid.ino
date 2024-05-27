#include "rfid.h"

/**
 * @brief Reads a card/tag RFID from the RFID antenna and assigns it to uidBytes
 * 
 * @param uidBytes The array which the bytes of the UID will be saved to
 * @param uidSize Size of the array, can get directly from RFID library
 */
void readCardUID(byte *uidBytes, int &uidSize) {
    for (byte i = 0; i < uidSize; i++) {
        uidBytes[i] = rfidReader.uid.uidByte[i];
    }
}