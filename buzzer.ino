#include "buzzer.h"

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

void playStartupBeep() {
    playSound(C, 500);
    playSound(E, 500);
}