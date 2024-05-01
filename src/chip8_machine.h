#ifndef _CHIP8_MACHINE_H_
#define _CHIP8_MACHINE_H_

#include <stdint.h>
#include <stdbool.h>

constexpr unsigned START_INSTRUCTION_ADDRESS = 0x200;
constexpr unsigned FONT_START_ADDRESS = 0x50;

constexpr unsigned CHIP8_SCREEN_WIDTH = 64;
constexpr unsigned CHIP8_SCREEN_HEIGHT = 32;
constexpr unsigned CHIP8_SCREEN_BUFFER_SIZE = 64 * 32;
constexpr unsigned CHIP8_SCREEN_BUFFER_SIZE_IN_BYTES = CHIP8_SCREEN_BUFFER_SIZE * sizeof(bool);

constexpr unsigned CHIP8_MEMORY_SIZE = 4096;

#define BLACK_COLOR 0x141414FF
#define WHITE_COLOR 0xC3C3C3FF

typedef enum Chip8_Signal {
  /**
   * @brief representa que não houve sinal
   */
  NONE = 0,
  /**
   * @brief representa uma tentativa de decrementar o stack_pointer quando não
   * houve incremento.
   */
  STACK_UNDERFLOW,
} Chip8_Signal;

typedef struct Chip8_Machine {
  // 16 registradores de 8 bits, sendo que o 16º é usado para armazenar o resultados de operaçẽos
  uint8_t registers[16];
  // Registrador de índice (I)
  uint16_t index_register;
  // Contador PC
  uint16_t program_counter;
  // stack de 16 níveis
  uint16_t stack[16];
  // stack pointer (stack índice)
  uint8_t stack_pointer;
  // temporizador usado para acompanhar a passagem do tempo
  uint8_t delay_timer;
  // temporizador usado para controlar a emissão de sons
  uint8_t sound_timer;

  // Memória de 4096 bytes, range 0x000-0xFFF
  uint8_t memory[CHIP8_MEMORY_SIZE];

  // Memória de vídeo, por ser monocromático preciso de um valor binário para representar um 'pixel',
  // então estou usando 'true' para a cor 'acessa' e 'false' para cor 'apagada'
  bool screen_buffer[CHIP8_SCREEN_BUFFER_SIZE];

  /**
   * Lista de 'keys' e estado de cada 'key', 'true' para pressionado e 'false' para quando não estiver pressionado.
   * A tecla '0' é representada pelo índice '0' no vetor, e assim por diante, cada tecla mapeando para um índice.
   * O layout do teclado do 'COSMAC VIP' é o seguinte:
   * |1|2|3|C|
   * |4|5|6|D|
   * |7|8|9|E|
   * |A|0|B|F|
   */
  bool keypad_state[16];

  // outras coisas
  uint16_t last_opcode_executed;

  Chip8_Signal last_opcode_signal;

  // acumulador
  uint16_t delay_timer_hertz_accumulator;
  
} Chip8_Machine;

#endif // _CHIP8_MACHINE_H_
