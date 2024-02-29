#pragma once

#include <stdint.h>

typedef struct Chip8_Machine {
  // 16 registradores de 8 bits, sendo que o 16º é usado para armazenar o resultados de operaçẽos
  uint8_t registers[16];
  // Registrador de índice
  uint16_t index_register;
  // Contador PC
  uint16_t program_counter;
  // stack de 16 níveis
  uint16_t stack[16];
  // stack pointer (stack índice)
  uint8_t stack_pointer;
  // Memória de 4096 bytes, range 0x000-0xFFF
  uint8_t memory[4096];

  // Memória de vídeo, estou usando cores 'rgba', um byte por canal, apesar de ser monocromático na prática
  uint32_t screen_buffer[64 * 32];
  // outras coisas
} Chip8_Machine;
