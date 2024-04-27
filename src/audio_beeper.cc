#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "./audio_beeper.h"

void generate_beep_samples_on_demand(Audio_Beeper *beeper , int16_t *audio_stream, const int length)
{
  if (!beeper->is_beeping)
  {
    // memset aqui, acho que SDL tem um menset cross plataform
    for (int i = 0; i < length; i++)
    {
      audio_stream[i] = 0;
    }

    return;
  }

  for (int i = 0; i < length; i++)
  {
    constexpr float volume_scale = 0.12;
    audio_stream[i] = AMPLITUDE * std::sin(beeper->v * 2 * M_PI / FREQUENCY) * volume_scale;
    beeper->v += beeper->frequency;
  }
}


/**
 * @brief Função callback passado ao audio device exposto pelo SDL
 * 
 * @param user_data contém a referência para a instância beeper que foi usada para configurar
 * dispositivo de audio
 * @param _audio_stream 
 * @param _length 
 */
void beeper_audio_callback(void * user_data, uint8_t *_audio_stream, int _length)
{
  assert(user_data != NULL);
  assert(_audio_stream != NULL);

  Audio_Beeper *beeper = (Audio_Beeper *) user_data;
  // pelo que entendi o formato real dependendo do parâmetro passado ao atributo format do `SDL_AudioSpec`
  int16_t *audio_stream = (int16_t *) _audio_stream;

  assert(_length % 2 == 0);
  int length = _length / 2;

  // @todo João, revisar se é necessário alguma forma de lock
  generate_beep_samples_on_demand(beeper, audio_stream, length);
}

