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
  // https://lospec.com/palette-list/paperback-2
  Color_Palette(0xB8C2B9FF, 0x382B26FF),
  // https://lospec.com/palette-list/1bit-monitor-glow
  Color_Palette(0x222323ff, 0xf0f6f0FF),
  // https://lospec.com/palette-list/ibm-51
  Color_Palette(0x323c39FF, 0xd3c9a1FF),
  Color_Palette(0xd3c9a1FF, 0x323c39FF),
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
