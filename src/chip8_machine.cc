#pragma once

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>

#include "./utils.macro.h"

constexpr unsigned START_INSTRUCTION_ADDRESS = 0x200;
constexpr unsigned FONT_START_ADDRESS = 0x50;

constexpr unsigned CHIP8_SCREEN_WIDTH = 64;
constexpr unsigned CHIP8_SCREEN_HEIGHT = 32;
constexpr unsigned CHIP8_SCREEN_BUFFER_SIZE = 64 * 32;
constexpr unsigned CHIP8_SCREEN_BUFFER_SIZE_IN_BYTES = CHIP8_SCREEN_BUFFER_SIZE * 4;

constexpr unsigned CHIP8_MEMORY_SIZE = 4096;

#define BLACK_COLOR 0x000000FF

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
  // temporizador usado para acompanhar a passagem do tempo
  uint8_t delay_timer;
  // temporizador usado para controlar a emissão de sons
  uint8_t sound_timer;

  // Memória de 4096 bytes, range 0x000-0xFFF
  uint8_t memory[CHIP8_MEMORY_SIZE];

  // Memória de vídeo, estou usando cores 'rgba', um byte por canal, apesar de ser monocromático na prática
  uint32_t screen_buffer[CHIP8_SCREEN_BUFFER_SIZE];
  // outras coisas
} Chip8_Machine;

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
static Chip8_Instruction_Execution_Code base_instruction_jump_table[16] = {};
static Chip8_Instruction_Execution_Code index_0_instruction_jump_table[16] = {};

/**
 * @brief CLS - Clear de display
 * 
 */
void execute_op_00E0(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  UNUSED(opcode);
  memset(chip8_machine->memory, 0, CHIP8_SCREEN_BUFFER_SIZE_IN_BYTES);
}

/**
 * @brief RET - Return from a subroutine
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_00EE(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  // @todo João, analisar se faz sentido colocar um if pra bloquear o o underflow da stack pointer

  chip8_machine->stack_pointer -= 1; // @note validar se está 100% decrementar antes
  chip8_machine->program_counter = chip8_machine->stack[chip8_machine->stack_pointer];
}

/**
 * @brief JP Address - Jump to location nnn
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_1nnn(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  // @todo João, tá estranho, parece estar em loop
  chip8_machine->program_counter = opcode & 0x0FFFu;
}

void decode_0_index_opcode(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  // Eventualmente talvez vou usar essa função pra fazer algum tipo de assert?
  uint8_t index = opcode & 0x000Fu; // @note Testar e revisar
  assert(index < 17);
  printf("decoding 0, index: %d\n", index);

  index_0_instruction_jump_table[index](chip8_machine, opcode);
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

  for (unsigned i = 0; i < 16; i++)
  {
    base_instruction_jump_table[i] = noop;
    index_0_instruction_jump_table[i] = noop;
  }

  // Primeira nível
  base_instruction_jump_table[0x0] = decode_0_index_opcode;
  base_instruction_jump_table[0x1] = execute_op_1nnn;

  // Segunda nível
  index_0_instruction_jump_table[0x0] = execute_op_00E0;
  index_0_instruction_jump_table[0xE] = execute_op_00EE;

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

  // incrementando o program counter
  chip8_machine.program_counter += 2;

  uint8_t index = (opcode & 0xF000u) >> 12u;
  assert(index < 17);

  base_instruction_jump_table[index](&chip8_machine, opcode);

  /**
   * @note Pelo que entendi vou precisar ajustar minha jump table para apontar para instruções
   * em alguns casos e em outros, a função que será executada será um segundo nível de decode, que
   * acionará a instrução correta. Portanto, em alguns casos será um salto direto para a função
   * que computa o efeito desejado pelo opcode, e em outros casos será um segunda função de decode
   * que fará o dispatch, criando assim um sistema flexível que pode ter vários níveis e e ainda assim
   * ter salto diretos.
   */

  printf("opcode: 0x%X, index: %d, pc: %d\n", opcode, index, chip8_machine.program_counter); // apenas para visualização
}
