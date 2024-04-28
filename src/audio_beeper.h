#ifndef _AUDIO_BEEPER_H_
#define _AUDIO_BEEPER_H_

#include <stdbool.h>

constexpr int AMPLITUDE = 28000;
constexpr int FREQUENCY = 44100;

typedef struct Audio_Beeper {
  volatile bool is_beeping;
  const double frequency;
  volatile double v; // @note Não entendi o que o 'v' representa
} Audio_Beeper;

#define INITIAL_AUDIO_BEEPER(FREQUENCY) { .is_beeping = false, .frequency = FREQUENCY, .v = 0, }

#endif // _AUDIO_BEEPER_H_
