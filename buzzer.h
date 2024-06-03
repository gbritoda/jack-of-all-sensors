#ifndef _BUZZER_H_
#define _BUZZER_H_

#define C 523
#define D_SHARP 622
#define E 659

#define BUZZER_PIN 23

/**
 * @brief Plays a sound in the buzzer
 * 
 * @param f Frequency of the tone
 * @param duration Time to play the tone in miliseconds
 */
void playSound(int f, int duration);

void playFalloutTheme();

void playConfirmationBeep();

void playReturnBeep();

void playStartupBeep();

#endif
