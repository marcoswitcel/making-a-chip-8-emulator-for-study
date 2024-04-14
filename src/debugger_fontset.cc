#pragma once

#include <stdint.h>
#include <stdbool.h>

constexpr int DEBUGGER_FONT_SET_SIZE = 128 * 5;

static const bool debbuger_fontset_initted = false;
static const uint8_t debbuger_fontset[DEBUGGER_FONT_SET_SIZE] = {
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0x00, 0x80, 0x00, 0x00, 0x80, // :
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80, // F
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF0, 0x80, 0x80, 0x80, 0xF0, // c
  0xE0, 0x90, 0x90, 0x90, 0xE0, // d
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0x20, 0x00, 0x20, 0x20, 0x20, // i
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x88, 0x88, 0xF8, // o
  0xF8, 0x88, 0xF8, 0x80, 0x80, // p
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF0, 0x90, 0xE0, 0x90, 0x90, // r
  0xF8, 0x80, 0xF8, 0x08, 0xF8, // s
  0xF0, 0x40, 0x40, 0x40, 0x40, // t
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
  0xF8, 0x88, 0x38, 0x20, 0x20, // ?
};
