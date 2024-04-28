#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "./audio_beeper.h"

void generate_beep_samples_on_demand(volatile Audio_Beeper *beeper , int16_t *audio_stream, const int length)
{
  if (!beeper->is_beeping)
  {
    memset(audio_stream, 0, length * sizeof(int16_t));

    return;
  }
  
  /**
   * @note Mesmo que o atributo `Audio_Beeper::is_beeping` seja atualizado no meio do processo de emissão
   * do som, vou terminar de emitir samples para preencher esse stream. Por hora não vi necessidade se ser tão
   * preciso.
   */

  for (int i = 0; i < length; i++)
  {
    constexpr float volume_scale = 0.12;
    audio_stream[i] = AMPLITUDE * std::sin(beeper->v * 2 * M_PI / FREQUENCY) * volume_scale;
    beeper->v += beeper->frequency;
  }
}


/**
 * @brief Função callback passado ao dispositivo de audio exposto pelo SDL
 * 
 * @param user_data contém a referência para a instância beeper que foi usada para configurar
 * dispositivo de audio
 * @param _audio_stream referência do stream passado ao callback, pode ser promovido a um tipo
 * mais específico de ponteiro de acordo com as configurações do dispositivo de áudio inicializado.
 * @param _length comprimento em bytes dos `audio_stream`, deve ser considerado a devida conversão
 * caso o stream seja promovido a um ponteiro de tipo maior.
 */
void beeper_audio_callback(void * user_data, uint8_t *_audio_stream, int _length)
{
  assert(user_data != NULL);
  assert(_audio_stream != NULL);

  Audio_Beeper *beeper = (Audio_Beeper *) user_data;
  // pelo que entendi o formato real depende do parâmetro passado ao atributo `SDL_AudioSpec::format`
  int16_t *audio_stream = (int16_t *) _audio_stream;

  assert(_length % 2 == 0);
  int length = _length / 2;

  // @todo João, revisar se é necessário alguma forma de lock
  generate_beep_samples_on_demand(beeper, audio_stream, length);
}


/**
 * @brief Configura e inicializa o dispositivo de áudio para o beeper
 * 
 * @param audio_beeper 
 */
void setup_audio_device_for_beeper(Audio_Beeper *audio_beeper)
{
  SDL_AudioSpec intended_spec;

  intended_spec.freq = FREQUENCY;
  intended_spec.format = AUDIO_S16SYS;
  intended_spec.channels = 1;
  intended_spec.samples = 2048;
  intended_spec.format = AUDIO_S16SYS;
  intended_spec.userdata = audio_beeper;
  intended_spec.callback = beeper_audio_callback;

  /**
   * @note Seria interessante validar em que situações o sistema pode falhar em
   * criar um dispositivo conforme a especificação e como lidar com isso de forma
   * graciosa.
   * @note Enfatizando, não foi testado com sistema que não adere a especificação
   * descrita acima e também não foi desenvolvido nenhum mecanismo de fallbackm
   * ou encerramento gracioso.
   */
  SDL_AudioSpec real_spec;
  assert(SDL_OpenAudio(&intended_spec, &real_spec) == 0);
  assert(real_spec.format == AUDIO_S16SYS);

  SDL_PauseAudio(0);
}
