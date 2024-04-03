#pragma once

#include <stdlib.h>

#include <SDL2/SDL.h>

#include "./chip8_machine.cc"
#include "./utils.macro.h"

static constexpr int WIDTH = 1024;
static constexpr int HEIGHT = 728;
static unsigned UI_TICKS_PER_SECOND = 60;

static bool is_debugging = false;
static bool run_next_step = false;

typedef struct Context_Data {
  bool clicked;
  int32_t last_clicked_x;
  int32_t last_clicked_y;
  int32_t mouse_x;
  int32_t mouse_y;
} Context_Data;

static void render_scene(SDL_Renderer *renderer, Chip8_Machine *chip8_machine, Context_Data *context)
{
  UNUSED(context);

  // Seta o fundo do renderer
  SDL_RenderClear(renderer);

  SDL_Rect area = {
    .x = 0,
    .y = 0,
    .w = WIDTH,
    .h = HEIGHT,
  };

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &area);


  // Copiando um buffer para a tela
  // @exemplo https://gamedev.stackexchange.com/questions/102490/fastest-way-to-render-image-data-from-buffer
  SDL_Texture *chip8_screen_memory = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
  void *pixels = NULL;
  int pitch;
  SDL_LockTexture(chip8_screen_memory, NULL, &pixels, &pitch);

  memcpy(pixels, chip8_machine->screen_buffer, 64 * 32 * 4);

  SDL_UnlockTexture(chip8_screen_memory);

  constexpr int scale_factor = 14; // temporário?
  SDL_Rect dest = { 10, 10, 64 * scale_factor, 32 * scale_factor };
  SDL_RenderCopy(renderer, chip8_screen_memory, NULL, &dest);

  SDL_RenderPresent(renderer);
}


static void handle_events_and_inputs(SDL_Window *window, Context_Data *context, bool *should_quit, Chip8_Machine *chip8_machine)
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
            // toggle modo depuração
            case SDLK_b: {
              is_debugging = !is_debugging;
            } break;
            // avança uma instrução se estiver debugando
            case SDLK_n: {
              if (is_debugging)
              {
                run_next_step = true;
              }
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
  printf("is loaded: %d\n", is_loaded);

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
  };

  SDL_Window *window = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    fprintf(stderr, "SDL não pode inicializar corretamente: %s\n", SDL_GetError());
  }

  window = SDL_CreateWindow(
    "Chip-8 Emulador: Executando",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WIDTH,
    HEIGHT,
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
     * @note Por hora decidi executa 30 instruções (30 ciclos), antes de fazer o flush para a tela.
     * Mas seria interessante analisar se tem alguma orientação geral sobre isso.
     */
    uint8_t execute_n_cycles = 30;

    if (is_debugging)
    {
      if (run_next_step)
      {
        execute_n_cycles = 1;
        run_next_step = false;
      }
      else
      {
        /**
         * @todo Por hora executo toda a rotina de processamento de input, porém, seria interessante
         * separar o input da UI do input do interpretador.
         */
        handle_events_and_inputs(window, &context, &should_quit, &chip8_machine);
        execute_n_cycles = 0;
      }
    }

    /**
     * @note Decidi buscar e atualizar input a cada ciclo, caso tenha algum dado novo de input,
     * mas uma otimização que poderia ser feita, é só buscar atualizar os dados de input se for usada
     * alguma das instruções que leem o estado do 'keypad'.
     */
    for (uint8_t i = 0; i < execute_n_cycles; i++)
    {
      // Processa eventos e inputs aqui
      handle_events_and_inputs(window, &context, &should_quit, &chip8_machine);

      // @todo João, definir como será feito a passagem de input
      // - Uma ideia seria usar um método que atualiza o input para o frame ou a nível de ciclo
      // - Criar um método de faz o submit do 'evento' de 'press down' e 'released'?
      execute_a_cycle(chip8_machine);
    }

    // Renderiza
    render_scene(renderer, &chip8_machine, &context);
    
    SDL_Delay(1000 / UI_TICKS_PER_SECOND);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  printf("Aplicação encerrada com sucesso.\n");

  return EXIT_SUCCESS;
}
