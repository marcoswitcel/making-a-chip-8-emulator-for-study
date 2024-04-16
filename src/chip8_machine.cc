#pragma once

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <fstream>

#include "./utils.macro.h"
#include "./chip8_machine.h"
#include "./chip8_instruction.cc"

constexpr int FONT_SET_SIZE = 80;

static const uint8_t fontset[FONT_SET_SIZE] = {
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
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void init_jump_table(); // por hora fica aqui em cima

static inline void reset_machine(Chip8_Machine &chip8_machine)
{
  // @note talvez mover para um construtor
  init_jump_table();

  chip8_machine.program_counter = START_INSTRUCTION_ADDRESS;

  // copiando dados da fonte para memória
  for (unsigned i = 0; i < FONT_SET_SIZE; i++)
  {
    chip8_machine.memory[FONT_SET_SIZE + i] = fontset[i];
  }

  for (unsigned i = 0; i < 16; i++)
  {
    chip8_machine.keypad_state[i] = false;
  }

  chip8_machine.delay_timer_hertz_accumulator = 0;
}

static inline void clearing_screen_buffer(Chip8_Machine &chip8_machine)
{
  // Preenchendo o buffer da tela com a cor preta
  for (unsigned i = 0; i < CHIP8_SCREEN_BUFFER_SIZE; i++)
  {
    chip8_machine.screen_buffer[i] = BLACK_COLOR; 
  }
}

static bool load_rom(Chip8_Machine &chip8_machine, const char *filename)
{
  // abre o arquivo já com o cursor do arquivo no final graças a flag `std::ios::ate`
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file.is_open()) return false;

  const std::streampos size_of_content = file.tellg();
  char *content_buffer = (char *) malloc(size_of_content * sizeof(char));

  file.seekg(0, std::ios::beg); // volta o cursor para o início do arquivo
  file.read(content_buffer, size_of_content);
  file.close();

  // move os dados do buffer de conteúdo para a memória da máquina no endereço correto
  // @note Aqui podeira passar o endereço da memória da máquina para a função read?
  for (long i = 0; i < size_of_content; i++)
  {
    chip8_machine.memory[START_INSTRUCTION_ADDRESS + i] = content_buffer[i];
  }

  free(content_buffer);

  return true;
}

// Instruções

typedef void (*Chip8_Instruction_Execution_Code)(Chip8_Machine *chip8_machine, uint16_t opcode);

static bool jump_table_inited = false;
static Chip8_Instruction_Execution_Code base_instruction_jump_table[0x0F + 1] = {};
static Chip8_Instruction_Execution_Code index_0_instruction_jump_table[0x0F + 1] = {};
static Chip8_Instruction_Execution_Code index_8_instruction_jump_table[0x0F + 1] = {};
static Chip8_Instruction_Execution_Code index_E_instruction_jump_table[0x0F + 1] = {};
static Chip8_Instruction_Execution_Code index_F_instruction_jump_table[0x65 + 1] = {};


void decode_0_index_opcode(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  // Eventualmente talvez vou usar essa função pra fazer algum tipo de assert?
  uint8_t index = opcode & 0x000Fu; // @note Testar e revisar
  printf("decoding 0, index: %d\n", index);

  index_0_instruction_jump_table[index](chip8_machine, opcode);
}

void decode_8_index_opcode(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  // Eventualmente talvez vou usar essa função pra fazer algum tipo de assert?
  uint8_t index = opcode & 0x000Fu; // @note Testar e revisar
  printf("decoding 8, index: %d\n", index);

  index_8_instruction_jump_table[index](chip8_machine, opcode);
}

void decode_E_index_opcode(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  // Eventualmente talvez vou usar essa função pra fazer algum tipo de assert?
  uint8_t index = opcode & 0x000Fu; // @note Testar e revisar
  printf("decoding E, index: %d\n", index);

  index_E_instruction_jump_table[index](chip8_machine, opcode);
}

void decode_F_index_opcode(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  // Eventualmente talvez vou usar essa função pra fazer algum tipo de assert?
  uint8_t index = opcode & 0x00FFu; // @note Testar e revisar
  printf("decoding F, index: %d\n", index);

  index_F_instruction_jump_table[index](chip8_machine, opcode);
}

void noop(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  UNUSED(chip8_machine);
  UNUSED(opcode);
  // Eventualmente talvez vou usar essa função pra fazer algum tipo de assert?
  printf("noop...\n");
}

void init_jump_table()
{
  if (jump_table_inited) return;

  for (unsigned i = 0; i <= 0xF; i++)
  {
    base_instruction_jump_table[i] = noop;
    index_0_instruction_jump_table[i] = noop;
    index_8_instruction_jump_table[i] = noop;
    index_E_instruction_jump_table[i] = noop;
  }

  for (unsigned i = 0; i <= 0x65; i++)
  {
    index_F_instruction_jump_table[i] = noop;
  }

  // Primeiro nível
  base_instruction_jump_table[0x0] = decode_0_index_opcode;
  base_instruction_jump_table[0x1] = execute_op_1nnn;
  base_instruction_jump_table[0x2] = execute_op_2nnn;
  base_instruction_jump_table[0x3] = execute_op_3xkk;
  base_instruction_jump_table[0x4] = execute_op_4xkk;
  base_instruction_jump_table[0x5] = execute_op_5xy0;
  base_instruction_jump_table[0x6] = execute_op_6xkk;
  base_instruction_jump_table[0x7] = execute_op_7xkk;
  base_instruction_jump_table[0x8] = decode_8_index_opcode;
  base_instruction_jump_table[0x9] = execute_op_9xy0;
  base_instruction_jump_table[0xA] = execute_op_Annn;
  base_instruction_jump_table[0xB] = execute_op_Bnnn;
  base_instruction_jump_table[0xC] = execute_op_Cxkk;
  base_instruction_jump_table[0xD] = execute_op_Dxyn;
  base_instruction_jump_table[0xE] = decode_E_index_opcode;
  base_instruction_jump_table[0xF] = decode_F_index_opcode;
  
  // Segundo nível (tabela 0)
  index_0_instruction_jump_table[0x0] = execute_op_00E0;
  index_0_instruction_jump_table[0xE] = execute_op_00EE;

  // Segundo nível (tabela 8)
  index_8_instruction_jump_table[0x0] = execute_op_8xy0;
  index_8_instruction_jump_table[0x1] = execute_op_8xy1;
  index_8_instruction_jump_table[0x2] = execute_op_8xy2;
  index_8_instruction_jump_table[0x3] = execute_op_8xy3;
  index_8_instruction_jump_table[0x4] = execute_op_8xy4;
  index_8_instruction_jump_table[0x5] = execute_op_8xy5;
  index_8_instruction_jump_table[0x6] = execute_op_8xy6;
  index_8_instruction_jump_table[0x7] = execute_op_8xy7;
  index_8_instruction_jump_table[0xE] = execute_op_8xyE;

  // Segundo nível (tabela E)
  index_E_instruction_jump_table[0x1] = execute_op_ExA1;
  index_E_instruction_jump_table[0xE] = execute_op_Ex9E;

  // Segundo nível (tabela F)
  index_F_instruction_jump_table[0x07] = execute_op_Fx07;
  index_F_instruction_jump_table[0x0A] = execute_op_Fx0A;
  index_F_instruction_jump_table[0x15] = execute_op_Fx15;
  index_F_instruction_jump_table[0x18] = execute_op_Fx18;
  index_F_instruction_jump_table[0x1E] = execute_op_Fx1E;
  index_F_instruction_jump_table[0x29] = execute_op_Fx29;
  index_F_instruction_jump_table[0x33] = execute_op_Fx33;
  index_F_instruction_jump_table[0x55] = execute_op_Fx55;
  index_F_instruction_jump_table[0x65] = execute_op_Fx65;

  jump_table_inited = true;
}

void execute_a_cycle(Chip8_Machine &chip8_machine)
{
  if (chip8_machine.program_counter >= CHIP8_MEMORY_SIZE)
  {
    printf("programa terminado...\n");
    return;
  }

  const uint8_t *memory = chip8_machine.memory; // alias
  
  // fetch opcode, como possuem dois bytes é necessário fazer o SHIFT e o OR
  uint16_t opcode = (memory[chip8_machine.program_counter] << 8u) | memory[chip8_machine.program_counter + 1];
  chip8_machine.last_opcode_executed = opcode;

  // incrementando o program counter
  chip8_machine.program_counter += 2;

  uint8_t index = (opcode & 0xF000u) >> 12u;
  assert(index < 17);

  base_instruction_jump_table[index](&chip8_machine, opcode);

  // manter o registro de quantos ciclos se passaram para poder acionar o decréscimo dos 'timers'
  chip8_machine.delay_timer_hertz_accumulator += 1;

  /**
   * @todo João, avaliar se não preciso sincronizar o decréscimo desses contadores considerando alguma taxa por segundo,
   * pois os jogos estão com a temporização meio imprecisa.
   * @todo João, avaliar por que as instruções que envolvem a rotação e a translação no programa de teste são tão lentas.
   * Geralmente envolvem a adição e decréscimo da stack.
   */

  // a cada 10 ciclos reseto o acumulador e tento decrementar os timers
  if (chip8_machine.delay_timer_hertz_accumulator == 10)
  {
    chip8_machine.delay_timer_hertz_accumulator = 0;

    if (chip8_machine.delay_timer > 0)
    {
      chip8_machine.delay_timer--;
    }

    if (chip8_machine.sound_timer > 0)
    {
      chip8_machine.sound_timer--;
    }
  }

  /**
   * @note Pelo que entendi vou precisar ajustar minha jump table para apontar para instruções
   * em alguns casos e em outros, a função que será executada será um segundo nível de decode, que
   * acionará a instrução correta. Portanto, em alguns casos será um salto direto para a função
   * que computa o efeito desejado pelo opcode, e em outros casos será um segunda função de decode
   * que fará o dispatch, criando assim um sistema flexível que pode ter vários níveis e e ainda assim
   * ter salto diretos.
   */

  printf("opcode: 0x%X, i: %d, pc: %d, sp: %d\n", opcode, chip8_machine.index_register, chip8_machine.program_counter, chip8_machine.stack_pointer); // apenas para visualização
}
