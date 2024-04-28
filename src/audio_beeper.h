#ifndef _AUDIO_BEEPER_H_
#define _AUDIO_BEEPER_H_

#include <stdbool.h>

constexpr int AMPLITUDE = 28000;
constexpr int FREQUENCY = 44100;

/**
 * @brief Estrutura que representa o `Beeper` que está sendo executado
 * 
 */
typedef struct Audio_Beeper {
  /**
   * @brief define se `Audio_Beeper` está ou não emitindo som, oque é executado
   * pela função callback passada ao sistema de som do SDL.
   */
  volatile bool is_beeping;
  /**
   * @brief Define a frequência sonora emitida,
   * 
   */
  const double frequency;
  /**
   * @brief Um acumulador para manter o registro e a progressão do som.
   */
  volatile double v;
} Audio_Beeper;

#define INITIAL_AUDIO_BEEPER(FREQUENCY) { .is_beeping = false, .frequency = FREQUENCY, .v = 0, }

#endif // _AUDIO_BEEPER_H_
