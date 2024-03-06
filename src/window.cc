#pragma once

#include <stdlib.h>

#include <SDL2/SDL.h>

#include "./chip8_machine.cc"
#include "./utils.macro.h"

static constexpr int WIDTH = 1024;
static constexpr int HEIGHT = 728;
static unsigned UI_TICKS_PER_SECOND = 24;

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


static void handle_events_and_inputs(SDL_Window *window, Context_Data *context, bool *should_quit)
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
      case SDL_MOUSEMOTION: {
        printf("motion: x %d, y %d\n", event.motion.x, event.motion.y);
        context->mouse_x = event.motion.x;
        context->mouse_y = event.motion.y;
      } break;
    }
  }
#pragma GCC diagnostic pop
}

int open_window(void)
{
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
    "Chip-8: Emulando",
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
  uint32_t last_timestamp = 0;

  Chip8_Machine chip8_machine;
  
  reset_machine(chip8_machine);
  clearing_screen_buffer(chip8_machine);

  // @note testando visualmente como fica o pixel scalonado
  chip8_machine.screen_buffer[0] = 0xFF0000FF;
  chip8_machine.screen_buffer[1] = 0x00FF00FF;
  chip8_machine.screen_buffer[2] = 0xFF00FFFF;

  /**
   * @brief usando ROM de teste
   * @reference
   * - https://github.com/corax89/chip8-test-rom/tree/master
   * - https://github.com/corax89/chip8-test-rom/blob/master/test_opcode.ch8
   */
  bool is_loaded = load_rom(chip8_machine, "test_opcode.ch8");
  printf("is loaded: %d\n", is_loaded);

  while (!should_quit)
  {
    // @note João, não achei a versão 64 bits na minha instalação
    // https://wiki.libsdl.org/SDL2/SDL_GetTicks
    uint32_t current_timestamp = SDL_GetTicks();
    float delta_time = (current_timestamp - last_timestamp) / 1000.0f;
    UNUSED(delta_time);

    // printf("delta_time: %f\n", delta_time);

    // Processa eventos e inputs aqui
    handle_events_and_inputs(window, &context, &should_quit);

    // Atualiza aqui
    last_timestamp = current_timestamp;

    execute_a_cycle(chip8_machine);

    // Renderiza
    render_scene(renderer, &chip8_machine, &context);
    
    SDL_Delay(1000 / UI_TICKS_PER_SECOND);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  printf("Aplicação encerrada com sucesso.\n");

  return EXIT_SUCCESS;
}
