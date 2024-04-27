#ifndef _AUDIO_BEEPER_H_
#define _AUDIO_BEEPER_H_

#include <stdbool.h>

constexpr int AMPLITUDE = 28000;
constexpr int FREQUENCY = 44100;

typedef struct Audio_Beeper {
  bool is_beeping;
  double frequency;
  double v; // @note Não entendi o que o 'v' representa
} Audio_Beeper;

#endif // _AUDIO_BEEPER_H_
