#pragma once

#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "./debugger_fontset.cc"
#include "./chip8_machine.cc"
#include "./utils.cc"
#include "./utils.macro.h"
#include "./audio_beeper.cc"
#include "./color_palette.h"

static constexpr int WINDOW_WIDTH = 1024;
static constexpr int WINDOW_HEIGHT = 512;
static unsigned UI_TICKS_PER_SECOND = 60;

static bool is_paused = false;
static bool is_debugging = false;
static bool show_debug_view = false;
static bool run_next_step = false;
static int run_n_steps = 0;

typedef struct Context_Data {
  bool clicked;
  int32_t last_clicked_x;
  int32_t last_clicked_y;
  int32_t mouse_x;
  int32_t mouse_y;
  bool fullscreen;
} Context_Data;

constexpr uint32_t RGBA_RED = 0xFF0000FF;
constexpr uint32_t RGBA_GREEN = 0x00FF00FF;
constexpr uint32_t RGBA_BLUE = 0x0000FFFF;
constexpr uint32_t RGBA_TRANSPARENT = 0x00000000;
constexpr uint32_t RGBA_WHITE = 0xFFFFFFFF;

/**
 * @brief 
 * 
 * @todo João, implementar um charset com letras, no estilo do charset padrão usado no interpretador
 * 
 * @param char_index 
 * @param buffer 
 * @param buffer_width 
 * @param buffer_height 
 * @param x 
 * @param y 
 * @param color 
 */
static void render_char(char char_index, uint32_t *buffer, uint32_t buffer_width, uint32_t buffer_height, uint32_t x, uint32_t y, uint32_t color)
{
  assert(char_index > -1 && char_index < 128); // @note precisa de um charset maior

  for (int row = 0; row < 5; row++)
  {
    const uint8_t sprite_byte = debbuger_fontset[char_index * 5 + row];

    for (int col = 0; col < 8; col++)
    {
      uint8_t pixel_on = sprite_byte & (0x80 >> col);
  
      if (pixel_on)
      {
        uint32_t index = (y + row) * buffer_width + (col + x);
        assert(index < buffer_width * buffer_height);
        
        if (index < buffer_width * buffer_height)
        {
          buffer[index] = color;
        }
      }
    }
  }
}

static void render_line(const char *cstring, uint32_t color, uint32_t *buffer, uint32_t buffer_width, uint32_t buffer_height, uint32_t x, uint32_t y)
{
  int length = strlen(cstring);

  for (int i = 0; i < length; i++)
  {
    int offset = i * 8;
    render_char(cstring[i], (uint32_t *) buffer, buffer_width, buffer_height, x + offset, y, color);
  }
}

static inline void render_full_overlay(SDL_Renderer *renderer)
{
  SDL_Rect overlay = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
  SDL_Color overlay_color = { 0, 0, 0, 200 };
  SDL_SetRenderDrawColor(renderer, overlay_color.r, overlay_color.g, overlay_color.b, overlay_color.a);
  SDL_RenderFillRect(renderer, &overlay);
} 

static void render_debug_panel(SDL_Renderer *renderer, Chip8_Machine *chip8_machine)
{
  render_full_overlay(renderer);

  SDL_Texture *debug_panel_view = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 256);
  void *pixels = NULL;
  int pitch;
  SDL_LockTexture(debug_panel_view, NULL, &pixels, &pitch);

  for (unsigned i = 0; i < 256 * 256; i++)
  {
    uint32_t *pixel = &((uint32_t*) pixels)[i];
    *pixel = RGBA_TRANSPARENT;
  }

  char label_sp[] = {'s', 'p', ':', '0', '\0'};
  label_sp[3] += chip8_machine->stack_pointer;
  render_line(label_sp, RGBA_RED, (uint32_t *) pixels, 256, 256, 0, 0);

  char *value = NULL;

  render_line("i:", RGBA_RED, (uint32_t *) pixels, 256, 256, 0, 6);
  value = int_to_cstring(chip8_machine->index_register);
  render_line(value, RGBA_RED, (uint32_t *) pixels, 256, 256, 16, 6);
  free(value);

  render_line("pc:", RGBA_RED, (uint32_t *) pixels, 256, 256, 0, 12);
  value = int_to_cstring(chip8_machine->program_counter);
  render_line(value, RGBA_RED, (uint32_t *) pixels, 256, 256, 24, 12);
  free(value);

  render_line("dt:", RGBA_RED, (uint32_t *) pixels, 256, 256, 0, 18);
  value = int_to_cstring(chip8_machine->delay_timer);
  render_line(value, RGBA_RED, (uint32_t *) pixels, 256, 256, 24, 18);
  free(value);

  render_line("st:", RGBA_RED, (uint32_t *) pixels, 256, 256, 0, 24);
  value = int_to_cstring(chip8_machine->sound_timer);
  render_line(value, RGBA_RED, (uint32_t *) pixels, 256, 256, 24, 24);
  free(value);

  /**
   * @note Acho que o melhor seria ter o painel de debug separado em diferentes 'viewers',
   * por exemplo, um visualizador de registradores, um visualizador de dados como PC, I e SP.
   * Um 'memory viewer'
   */
  for (int i = 0; i < 16; i++)
  {
    char label_rx[] = {'r', ' ', ':', '\0'};
    
    if (i > 9) label_rx[1] = (i - 10) + 'A';
    else label_rx[1] = i + '0';

    render_line(label_rx, RGBA_RED, (uint32_t *) pixels, 256, 256, 56, 5 * i + i);
    char *number = int_to_cstring(chip8_machine->registers[i]);
    render_line(number, RGBA_RED, (uint32_t *) pixels, 256, 256, 88, 5 * i + i);
    free(number);
  }

  char opcode[] = "op:";
  render_line(opcode, RGBA_RED, (uint32_t *) pixels, 256, 256, 0, 100);
  value = int_to_cstring_in_base(chip8_machine->last_opcode_executed, HEXADECIMAL);
  render_line(value, RGBA_RED, (uint32_t *) pixels, 256, 256, 32, 100);
  free(value);

  if (chip8_machine->last_opcode_signal != NONE)
  {
    char opcode[] = "signal:";
    render_line(opcode, RGBA_BLUE,(uint32_t *) pixels, 256, 256, 0, 106);
    const char *signal_name = get_chip8_signal_name(chip8_machine->last_opcode_signal);
    render_line(signal_name, RGBA_BLUE, (uint32_t *) pixels, 256, 256, 57, 106);
  }
  
  /**
   * render keypad
   * 
   * |1|2|3|C|
   * |4|5|6|D|
   * |7|8|9|E|
   * |A|0|B|F|
   * 
   */
  {
    const char digits_in_order[] = "123C456D789EA0BF";
    const int length = sizeof(digits_in_order) / sizeof(char) - 1;

    for (int i = 0; i < length; i++)
    {
      const int offset_x = 200;
      const int offset_y = 70;
      const int pos_x = offset_x + (i % 4) * 8;
      const int pos_y = offset_y + (i / 4) * 8;
      const char digit = digits_in_order[i];
      const int key_index = (digit < 'A') ? digit - '0' : digit - 'A' + 10;

      uint32_t color = (chip8_machine->keypad_state[key_index]) ? RGBA_GREEN : RGBA_RED;
      render_char(digits_in_order[i], (uint32_t *) pixels, 256, 256, pos_x, pos_y, color);
    }
  }

  SDL_UnlockTexture(debug_panel_view);

  // copia pro buffer visível
  SDL_Rect dest = {
    .x = 10, .y = 10,
    // .x = 0, .y = WINDOW_HEIGHT - (WINDOW_HEIGHT / 3),
    // .w = WINDOW_WIDTH, .h = WINDOW_HEIGHT / 3
    .w = 256 * 4, .h = 256 * 4
  };

  // @note para manter o canal alfa 
  // @reference https://stackoverflow.com/questions/29807872/sdl2-texture-render-target-doesnt-have-alpha-transparency
  SDL_SetTextureBlendMode(debug_panel_view, SDL_BLENDMODE_BLEND);

  SDL_RenderCopy(renderer, debug_panel_view, NULL, &dest);

  SDL_DestroyTexture(debug_panel_view);
}

/**
 * @brief Renderiza o texto na posição, com a cor e a escala definida
 * 
 * @todo João, ajustar na fonte pelo menos os caracteres de "pausado"
 * 
 * @param text 
 * @param renderer 
 * @param x 
 * @param y 
 * @param scale 
 * @param color
 */
static void render_text_to_screen(const char *text, SDL_Renderer *renderer, int x, int y, int scale, uint32_t color)
{
  const int length = strlen(text);
  const int width = length * 8;
  const int height = 5; 

  SDL_Texture *text_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
  void *pixels = NULL;
  int pitch;

  SDL_LockTexture(text_texture, NULL, &pixels, &pitch);

  // @note Parece que a textura já vem "limpa", porém por garantir zero todos os canais
  memset(pixels, 0, width * height * sizeof(uint32_t));

  render_line(text, color, (uint32_t *) pixels, width, height, 0, 0);

  SDL_UnlockTexture(text_texture);

  // copia pro buffer visível
  SDL_Rect dest = {
    .x = x - ((width * scale) / 2),
    .y = y - ((height * scale) / 2),
    .w = width * scale,
    .h = height * scale
  };

  SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);

  SDL_RenderCopy(renderer, text_texture, NULL, &dest);

  SDL_DestroyTexture(text_texture);
}

static void render_scene(SDL_Renderer *renderer, Chip8_Machine *chip8_machine, Context_Data *context)
{
  UNUSED(context);

  // Seta o fundo do renderer
  SDL_RenderClear(renderer);

  SDL_Rect area = {
    .x = 0,
    .y = 0,
    .w = WINDOW_WIDTH,
    .h = WINDOW_HEIGHT,
  };

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &area);


  // Copiando um buffer para a tela
  // @exemplo https://gamedev.stackexchange.com/questions/102490/fastest-way-to-render-image-data-from-buffer
  SDL_Texture *chip8_screen_memory = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
  void *pixels = NULL;
  int pitch;
  SDL_LockTexture(chip8_screen_memory, NULL, &pixels, &pitch);

  Color_Palette palette = get_current_color_palette();

  for (unsigned i = 0; i < CHIP8_SCREEN_BUFFER_SIZE; i++)
  {
    ((uint32_t *) pixels)[i] = chip8_machine->screen_buffer[i] ? palette.on : palette.off;
  }

  SDL_UnlockTexture(chip8_screen_memory);

  constexpr int scale_factor = 15; // temporário?
  SDL_Rect dest = {
    .x = (WINDOW_WIDTH / 2) - (CHIP8_SCREEN_WIDTH * scale_factor / 2),
    .y = (WINDOW_HEIGHT / 2) - (CHIP8_SCREEN_HEIGHT * scale_factor / 2),
    .w = CHIP8_SCREEN_WIDTH * scale_factor, .h = CHIP8_SCREEN_HEIGHT * scale_factor
  };
  SDL_RenderCopy(renderer, chip8_screen_memory, NULL, &dest);

  SDL_DestroyTexture(chip8_screen_memory);

  if (is_paused)
  {
    render_full_overlay(renderer);
    render_text_to_screen("Pausado", renderer, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 4, RGBA_WHITE);
  }
  else if (is_debugging || show_debug_view)
  {
    render_debug_panel(renderer, chip8_machine);

    if (is_debugging)
    {
      // linha vermelha no rodapé
      SDL_Rect overlay = { 0, WINDOW_HEIGHT - 10, WINDOW_WIDTH, 10 };
      SDL_Color overlay_color = { 255, 0, 0, 255 };
      SDL_SetRenderDrawColor(renderer, overlay_color.r, overlay_color.g, overlay_color.b, overlay_color.a);
      SDL_RenderFillRect(renderer, &overlay);
    }
  }

  SDL_RenderPresent(renderer);
}

static void handle_events_and_inputs(SDL_Window *window, SDL_Renderer *renderer, Context_Data *context, bool *should_quit, Chip8_Machine *chip8_machine)
{
  UNUSED(window);
  SDL_Event event;

  context->clicked = false;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
  while (SDL_PollEvent(&event))
  {
    printf("Processando evento:");
    printf("%d\n", event.type);
    switch (event.type)
    {
      case SDL_QUIT: *should_quit = true; break;
      case SDL_MOUSEBUTTONDOWN: {
        printf("Mouse button down\n");
        if (event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_PRESSED)
        {
          printf("clicked: x %d, y %d\n", event.button.x, event.button.y);
          context->clicked = true;
          context->last_clicked_x = event.button.x;
          context->last_clicked_y = event.button.y;
        }
      } break;
      case SDL_KEYDOWN: {
        assert(event.key.state == SDL_PRESSED);
        if (!event.key.repeat)
        {
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE: {
              *should_quit = true;
            } break;
            case SDLK_x: {
              chip8_machine->keypad_state[0] = 1;
            } break;
            case SDLK_1: {
              chip8_machine->keypad_state[1] = 1;
            } break;
            case SDLK_2: {
              chip8_machine->keypad_state[2] = 1;
            } break;
            case SDLK_3: {
              chip8_machine->keypad_state[3] = 1;
            } break;
            case SDLK_q: {
              chip8_machine->keypad_state[4] = 1;
            } break;
            case SDLK_w: {
              chip8_machine->keypad_state[5] = 1;
            } break;
            case SDLK_e: {
              chip8_machine->keypad_state[6] = 1;
            } break;
            case SDLK_a: {
              chip8_machine->keypad_state[7] = 1;
            } break;
            case SDLK_s: {
              chip8_machine->keypad_state[8] = 1;
            } break;
            case SDLK_d: {
              chip8_machine->keypad_state[9] = 1;
            } break;
            case SDLK_z: {
              chip8_machine->keypad_state[0xA] = 1;
            } break;
            case SDLK_c: {
              chip8_machine->keypad_state[0xB] = 1;
            } break;
            case SDLK_4: {
              chip8_machine->keypad_state[0xC] = 1;
            } break;
            case SDLK_r: {
              chip8_machine->keypad_state[0xD] = 1;
            } break;
            case SDLK_f: {
              chip8_machine->keypad_state[0xE] = 1;
            } break;
            case SDLK_v: {
              chip8_machine->keypad_state[0xF] = 1;
            } break;
            // toggle pausa
            case SDLK_p: {
              is_paused = !is_paused;
            } break;
            // restart emulação
            case SDLK_o: {
              // @todo João, avaliar como lidar com o recarregamento da ROM
              reset_machine(*chip8_machine);
              clearing_screen_buffer(*chip8_machine);
            } break;
            case SDLK_i: {
              next_color_palette();
            } break;
            // toggle fullscreen
            case SDLK_g: {
              /**
               * @todo João, revisar e testar mais isso aqui.. usei pouco o modo tela cheia do SDL
               * @note 'SDL_WINDOW_FULLSCREEN' tem o comportamento de trocar a resolução do monitor, mas não sei se isso é melhor
               * 
               * @link https://stackoverflow.com/questions/76623721/sdl-window-fullscreen-on-ubuntu
               * @link https://stackoverflow.com/questions/34520273/how-do-i-change-the-resolution-of-a-window-in-sdl-everything-is-too-large
               * @link https://stackoverflow.com/questions/30629106/sdl2-how-to-properly-toggle-fullscreen
               */

              if (context->fullscreen)
              {
                context->fullscreen = false;
                SDL_SetWindowFullscreen(window, 0);
              }
              else
              {
                context->fullscreen = true;
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                // @note João, não precisa restaurar isso aqui? supus que não afetaria o modo 'windowed',
                // mas não testei tão a fundo
                SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
              }
            } break;
            // toggle modo depuração
            case SDLK_b: {
              is_debugging = !is_debugging;
            } break;
            // toggle painel de debug
            case SDLK_COMMA: {
              show_debug_view = !show_debug_view;
            } break;
            // avança uma instrução se estiver debugando
            case SDLK_n: {
              if (is_debugging)
              {
                run_next_step = true;
              }
            } break;
            // avança 10 instruções se estiver debugando
            case SDLK_m: {
              if (is_debugging)
              {
                run_n_steps = 10;
              }
            } break;
          }
        }
      } break;
      case SDL_KEYUP: {
        if (!event.key.repeat)
        {
          switch (event.key.keysym.sym)
          {
            case SDLK_x: {
              chip8_machine->keypad_state[0] = 0;
            } break;
            case SDLK_1: {
              chip8_machine->keypad_state[1] = 0;
            } break;
            case SDLK_2: {
              chip8_machine->keypad_state[2] = 0;
            } break;
            case SDLK_3: {
              chip8_machine->keypad_state[3] = 0;
            } break;
            case SDLK_q: {
              chip8_machine->keypad_state[4] = 0;
            } break;
            case SDLK_w: {
              chip8_machine->keypad_state[5] = 0;
            } break;
            case SDLK_e: {
              chip8_machine->keypad_state[6] = 0;
            } break;
            case SDLK_a: {
              chip8_machine->keypad_state[7] = 0;
            } break;
            case SDLK_s: {
              chip8_machine->keypad_state[8] = 0;
            } break;
            case SDLK_d: {
              chip8_machine->keypad_state[9] = 0;
            } break;
            case SDLK_z: {
              chip8_machine->keypad_state[0xA] = 0;
            } break;
            case SDLK_c: {
              chip8_machine->keypad_state[0xB] = 0;
            } break;
            case SDLK_4: {
              chip8_machine->keypad_state[0xC] = 0;
            } break;
            case SDLK_r: {
              chip8_machine->keypad_state[0xD] = 0;
            } break;
            case SDLK_f: {
              chip8_machine->keypad_state[0xE] = 0;
            } break;
            case SDLK_v: {
              chip8_machine->keypad_state[0xF] = 0;
            } break;
          }
        }
      } break;
      case SDL_MOUSEMOTION: {
        printf("motion: x %d, y %d\n", event.motion.x, event.motion.y);
        context->mouse_x = event.motion.x;
        context->mouse_y = event.motion.y;
      } break;
    }
  }
#pragma GCC diagnostic pop
}

int open_window(const char *filename)
{
  Chip8_Machine chip8_machine;
  
  reset_machine(chip8_machine);
  clearing_screen_buffer(chip8_machine);

  /**
   * @brief usando ROM de teste
   * @reference
   * - https://github.com/corax89/chip8-test-rom/tree/master
   * - https://github.com/corax89/chip8-test-rom/blob/master/test_opcode.ch8
   */
  bool is_loaded = load_rom(chip8_machine, filename);

  if (!is_loaded)
  {
    printf("Erro ao carregar o arquivo '%s'\nEncerrando...\n", filename);
    return EXIT_FAILURE;
  }

  Context_Data context = {
    .clicked = false,
    .last_clicked_x = 0,
    .last_clicked_y = 0,
    .mouse_x = 0,
    .mouse_y = 0,
    .fullscreen = false,
  };

  SDL_Window *window = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    fprintf(stderr, "SDL não pode inicializar corretamente: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (SDL_Init(SDL_INIT_AUDIO) != 0)
  {
    // @todo João, validar o que ocorre se não conseguir inicializar o sistema de som,
    // checar se precisa encerrar.
    fprintf(stderr, "SDL não pode inicializar o sistema de som corretamente: %s\n", SDL_GetError());

    // @todo João, por hora vou encerrar porque ainda não vou abstrair e modularizar o sistema de som/beep
    return EXIT_FAILURE;
  }


  Audio_Beeper audio_beeper = INITIAL_AUDIO_BEEPER(550);

  setup_audio_device_for_beeper(&audio_beeper);

  const char *title_prefix = "Chip-8:  ";
  const int title_length = strlen(title_prefix) + strlen(filename);
  char *window_title = (char *) malloc((title_length + 1) * sizeof(char));

  strcpy(window_title, title_prefix);
  strcat(window_title, filename);

  window = SDL_CreateWindow(
    window_title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    0
  );

  if (!window)
  {
    fprintf(stderr, "Janela SDL não pode inicializar corretamente: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer)
  {
    fprintf(stderr, "O Renderer SDL não pode inicializar corretamente: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  bool should_quit = false;

  while (!should_quit)
  {
    /**
     * @note Por hora decidi executa 10 instruções (10 ciclos), antes de fazer o flush para a tela.
     * Mas seria interessante analisar se tem alguma orientação geral sobre isso.
     */
    uint8_t execute_n_cycles = 10;

    if (is_debugging)
    {
      if (run_next_step)
      {
        execute_n_cycles = 1;
        run_next_step = false;
      }
      else if (run_n_steps > 0)
      {
        execute_n_cycles = 10;
        run_n_steps = 0;
      }
      else
      {
        execute_n_cycles = 0;
      }
    }

    /**
     * @todo Por hora executo toda a rotina de processamento de input, porém, seria interessante
     * separar o input da UI do input do interpretador.
     */
    handle_events_and_inputs(window, renderer, &context, &should_quit, &chip8_machine);

    /**
     * @note Decidi buscar e atualizar os dados de input a cada ciclo, mas uma otimização que
     * poderia ser feita, é só buscar atualizar os dados de input se for usada alguma das
     * instruções que fazem a leitura do estado do 'keypad'.
     */
    if (!is_paused) for (uint8_t i = 0; i < execute_n_cycles; i++)
    {
      // Processa eventos e inputs aqui
      handle_events_and_inputs(window, renderer, &context, &should_quit, &chip8_machine);

      // @todo João, definir como será feito a passagem de input
      // - Uma ideia seria usar um método que atualiza o input para o frame ou a nível de ciclo
      // - Criar um método de faz o submit do 'evento' de 'press down' e 'released'?
      execute_a_cycle(chip8_machine);

      // atualizando beeper
      audio_beeper.is_beeping = chip8_machine.sound_timer > 0;

      // @todo João, considerar se faz sentido ter níveis de gravidade nos sinais e talvez
      // exibir alguns como alertas no canto da tela ao invés de exibir no depurador.
      if (chip8_machine.last_opcode_signal != NONE)
      {
        // Caso ative a depuração é necessário parar o loop para deixar o código do loop
        // externo ajustar a quantidade de ciclos para zero.
        is_debugging = true;
        break;
      }
    }

    // Renderiza
    render_scene(renderer, &chip8_machine, &context);
    
    SDL_Delay(1000 / UI_TICKS_PER_SECOND);
  }

  SDL_CloseAudio();
  SDL_DestroyWindow(window);
  SDL_Quit();
  printf("Aplicação encerrada com sucesso.\n");

  return EXIT_SUCCESS;
}
