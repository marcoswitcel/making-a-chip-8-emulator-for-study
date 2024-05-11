#ifndef _COLOR_PALETTE_H_
#define _COLOR_PALETTE_H_

#include <assert.h>
#include <stdint.h>

typedef struct Color_Palette {
  uint32_t on;
  uint32_t off;
} Color_Palette;

#define Color_Palette(ON, OFF) { .on = ON, .off = OFF, }

#define BLACK_COLOR 0x141414FF
#define WHITE_COLOR 0xC3C3C3FF

static int current_color_palette_index = 0;

static Color_Palette palettes[] = {
  Color_Palette(WHITE_COLOR, BLACK_COLOR),
  Color_Palette(BLACK_COLOR, WHITE_COLOR),
  // @todo João, revisar cores abaixos e adicionar mais palettas legais
  Color_Palette(0xFF0000FF, 0x00FF00FF),
  Color_Palette(0x00FF00FF, 0xFF0000FF),
};

static int palettes_size = sizeof(palettes) / sizeof(palettes[0]);

static inline Color_Palette get_current_color_palette()
{
  int index = current_color_palette_index;

  if (index >= palettes_size)
  {
    assert(index < palettes_size);
    index = 0;
  }

  return palettes[index];
}

static inline void next_color_palette(void)
{
  current_color_palette_index  = (current_color_palette_index + 1) % palettes_size;
}


#endif // _COLOR_PALETTE_H_
