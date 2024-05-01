#pragma once

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <fstream>

#include "./chip8_machine.h"
#include "./utils.macro.h"

static bool seed_inited = false;
static int random_seed = -1;

static uint8_t random_byte()
{
  // @todo João, setar o seed em outro lugar e pensar em como alimentá-lo melhor.
  // Talvez usar um outro método, tanto pela perfomance quanto pela qualidade dos valores.
  // Tem umas dicas pra gerar números aleatório no linux.
  // @reference https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c
  if (!seed_inited)
  {
    random_seed = time(NULL);
    seed_inited = true;
    srand(random_seed);
  }

  return rand() % 256;
}

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
  UNUSED(opcode);

  /**
   * @note a stack atualmente é representada por um array de 16 inteiros e um índice, é importante
   * manter o `stack_pointer` com valores de 0 a 15, incluindo ambas extremidades. Isso é importante 
   * para impedir acesso indevido de memória, até mesmo fora da memória da VM.
   */

  if (chip8_machine->stack_pointer > 0)
  {
    chip8_machine->stack_pointer -= 1;
    chip8_machine->program_counter = chip8_machine->stack[chip8_machine->stack_pointer];
  }
  else
  {
    chip8_machine->last_opcode_signal = STACK_UNDERFLOW;
  }
}

/**
 * @brief JP Address - Jump to location nnn
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_1nnn(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  chip8_machine->program_counter = opcode & 0x0FFFu;
}

/**
 * @brief CALL Address - Call subroutine at nnn
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_2nnn(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  if (chip8_machine->stack_pointer < CHIP8_STACK_SIZE)
  {
    // `program_counter` aponta para a próxima instrução após o comando CALL,
    // por isso não precisa incrementar aqui
    chip8_machine->stack[chip8_machine->stack_pointer] = chip8_machine->program_counter;
    // meu stack_pointer aponta para o próximo endereço disponível, por isso
    // pode ser incrementado após
    chip8_machine->stack_pointer += 1;

    chip8_machine->program_counter = opcode & 0x0FFFu;
  }
  else
  {
    chip8_machine->last_opcode_signal = STACK_OVERFLOW;
  }
}

/**
 * @brief Skip Equals Vx, byte - Skip next instruction if Vx = kk
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_3xkk(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t r_index = (opcode & 0x0F00u) >> 8u;
  uint8_t byte_value = (opcode & 0x00FFu);

  if (chip8_machine->registers[r_index] == byte_value)
  {
    chip8_machine->program_counter += 2;
  }
}

/**
 * @brief Skip Not Equals Vx, byte - Skip next instruction if Vx = kk
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_4xkk(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t r_index = (opcode & 0x0F00u) >> 8u;
  uint8_t byte_value = (opcode & 0x00FFu);

  if (chip8_machine->registers[r_index] != byte_value)
  {
    chip8_machine->program_counter += 2;
  }
}

/**
 * @brief Skip Equals Vx, Vy - Skip next instruction if Vx = Vy
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_5xy0(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  if (chip8_machine->registers[x_index] == chip8_machine->registers[y_index])
  {
    chip8_machine->program_counter += 2;
  }
}

/**
 * @brief Load Vx, byte - Set Vx = kk
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_6xkk(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t r_index = (opcode & 0x0F00u) >> 8u;
  uint8_t byte_value = (opcode & 0x00FFu);

  chip8_machine->registers[r_index] = byte_value;
}

/**
 * @brief Add Vx, byte - Set Vx = Vx + kk
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_7xkk(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t r_index = (opcode & 0x0F00u) >> 8u;
  uint8_t byte_value = (opcode & 0x00FFu);

  chip8_machine->registers[r_index] += byte_value;
}

/**
 * @brief Load Vx, Vy - Set Vx = Vy
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xy0(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  chip8_machine->registers[x_index] = chip8_machine->registers[y_index];
}

/**
 * @brief OR Vx, Vy - Set Vx = Vx OR Vy
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xy1(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  chip8_machine->registers[x_index] |= chip8_machine->registers[y_index];
}

/**
 * @brief AND Vx, Vy - Set Vx = Vx AND Vy
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xy2(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  chip8_machine->registers[x_index] &= chip8_machine->registers[y_index];
}

/**
 * @brief XOR Vx, Vy - Set Vx = Vx XOR Vy
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xy3(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  chip8_machine->registers[x_index] ^= chip8_machine->registers[y_index];
}

/**
 * @brief ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xy4(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  uint16_t result = chip8_machine->registers[x_index] + chip8_machine->registers[y_index];

  chip8_machine->registers[x_index] = result & 0x00FFu;
  // carry
  chip8_machine->registers[0xF] = (result & 0xFF00u) > 0;
  assert(chip8_machine->registers[0xF] == 1 || chip8_machine->registers[0xF] == 0);
}

/**
 * @brief SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xy5(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  // @note João, aqui não deveria incluir Vx == Vy como 'not borrow?
  // Parece que realmente não é um 'borrow' quando Vx == Vy.
  // @reference https://www.reddit.com/r/EmuDev/comments/kgy70l/chip8_8xy7_8xy5_behavior/
  // NOT borrow
  chip8_machine->registers[0xF] = chip8_machine->registers[x_index] > chip8_machine->registers[y_index];
  assert(chip8_machine->registers[0xF] == 1 || chip8_machine->registers[0xF] == 0);

  chip8_machine->registers[x_index] = chip8_machine->registers[x_index] - chip8_machine->registers[y_index];
}

/**
 * @brief SHR Vx {, Vy} - Set Vx = Vx SHR 1
 * Vy não é usado
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xy6(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  // salvando/retornando bit menos significativo
  chip8_machine->registers[0xF] = (chip8_machine->registers[x_index] & 0x01u);

  chip8_machine->registers[x_index] >>= 1;
}

/**
 * @brief SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xy7(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  // @note João, aqui não deveria incluir Vx == Vy como 'not borrow?
  // Parece que realmente não é um 'borrow' quando Vx == Vy.
  // @reference https://www.reddit.com/r/EmuDev/comments/kgy70l/chip8_8xy7_8xy5_behavior/
  // NOT borrow
  chip8_machine->registers[0xF] = chip8_machine->registers[y_index] > chip8_machine->registers[x_index];
  assert(chip8_machine->registers[0xF] == 1 || chip8_machine->registers[0xF] == 0);

  chip8_machine->registers[x_index] = chip8_machine->registers[y_index] - chip8_machine->registers[x_index];
}

/**
 * @brief SHL Vx {, Vy} - Set Vx = Vx SHL 1
 * Vy não é usado
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_8xyE(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  // salvando/retornando bit mais significativo
  chip8_machine->registers[0xF] = (chip8_machine->registers[x_index] & 0x80u) >> 7u;

  chip8_machine->registers[x_index] <<= 1;
}

/**
 * @brief Skip Not Equals Vx, Vy - Skip next instruction if Vx != Vy
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_9xy0(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;

  if (chip8_machine->registers[x_index] != chip8_machine->registers[y_index])
  {
    chip8_machine->program_counter += 2;
  }
}

/**
 * @brief Load I, addr -  Set I - nnn (I = Memory Index Register)
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Annn(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  chip8_machine->index_register = (opcode & 0x0FFFu);
}

/**
 * @brief Jump V0, addr - Jump to location nnn + V0 (register 0)
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Bnnn(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  chip8_machine->program_counter = chip8_machine->registers[0] + (opcode & 0x0FFFu);
}

/**
 * @brief Rand Vx, byte - Set Vx = random byte AND kk
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Cxkk(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t r_index = (opcode & 0x0F00u) >> 8u;
  uint8_t byte_value = (opcode & 0x00FFu);

  uint8_t rand_byte = random_byte();

  chip8_machine->registers[r_index] = rand_byte & byte_value;
}

/**
 * @brief Draw Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
 * 
 * @todo Adicionar testes... quando for interessante
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Dxyn(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t y_index = (opcode & 0x00F0u) >> 4u;
  uint8_t height_nibble = (opcode & 0x000Fu);

  // decidi implementar o 'wrap-around'
  uint8_t x_pos = chip8_machine->registers[x_index] % CHIP8_SCREEN_WIDTH;
  uint8_t y_pos = chip8_machine->registers[y_index] % CHIP8_SCREEN_HEIGHT;

  // return register
  chip8_machine->registers[0xF] = 0;

  for (uint16_t row = 0; row < height_nibble; row++)
  {
    assert(chip8_machine->index_register + row < CHIP8_MEMORY_SIZE);
    uint8_t sprite_byte = chip8_machine->memory[chip8_machine->index_register + row];

    for (unsigned col = 0; col < 8; col++)
    {
      uint8_t pixel_on = sprite_byte & (0x80 >> col);
      
      // @note João, sem 'warp-around' de cima pra baixo e de baixo pra cima
      if (y_pos + row >= CHIP8_SCREEN_HEIGHT) continue;

      assert((y_pos + row) * CHIP8_SCREEN_WIDTH + (x_pos + col) < CHIP8_SCREEN_BUFFER_SIZE);
      bool &pixel = chip8_machine->screen_buffer[(y_pos + row) * CHIP8_SCREEN_WIDTH + (x_pos + col)];
      
      if (pixel_on)
      {
        if (pixel)
        {
          chip8_machine->registers[0xF] = 1;
        }

        pixel = !pixel;
      }
    }
  }
}

/**
 * @brief Skip Key[Vx] Pressed - Skip next instruction if key with value of Vx is pressed 
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Ex9E(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  // @todo Ajustar o possível @out-of-boundary abaixo, pensar em como lidar e pesquisar como outros emuladores garantem isso 
  uint8_t key_value =  chip8_machine->registers[x_index]; 
  assert(key_value < 16);

  if (chip8_machine->keypad_state[key_value])
  {
    chip8_machine->program_counter += 2;
  }
}

/**
 * @brief Skip Not Key[Vx] Pressed - Skip next instruction if key with value of Vx is not pressed 
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_ExA1(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  // @todo Ajustar o possível @out-of-boundary abaixo, pensar em como lidar e pesquisar como outros emuladores garantem isso 
  uint8_t key_value =  chip8_machine->registers[x_index]; 
  assert(key_value < 16);

  if (!chip8_machine->keypad_state[key_value])
  {
    chip8_machine->program_counter += 2;
  }
}

/**
 * @brief Load Vx, DT - Set Vx = delay timer value
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx07(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  chip8_machine->registers[x_index] = chip8_machine->delay_timer;
}

/**
 * @brief Load Vx, Key - Wait for a key press, store the value of the key in Vx
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx0A(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  if (chip8_machine->keypad_state[0])
  {
    chip8_machine->registers[x_index] = 0;
  }
  else if (chip8_machine->keypad_state[1])
  {
    chip8_machine->registers[x_index] = 1;
  }
  else if (chip8_machine->keypad_state[2])
  {
    chip8_machine->registers[x_index] = 2;
  }
  else if (chip8_machine->keypad_state[3])
  {
    chip8_machine->registers[x_index] = 3;
  }
  else if (chip8_machine->keypad_state[4])
  {
    chip8_machine->registers[x_index] = 4;
  }
  else if (chip8_machine->keypad_state[5])
  {
    chip8_machine->registers[x_index] = 5;
  }
  else if (chip8_machine->keypad_state[6])
  {
    chip8_machine->registers[x_index] = 6;
  }
  else if (chip8_machine->keypad_state[7])
  {
    chip8_machine->registers[x_index] = 7;
  }
  else if (chip8_machine->keypad_state[8])
  {
    chip8_machine->registers[x_index] = 8;
  }
  else if (chip8_machine->keypad_state[9])
  {
    chip8_machine->registers[x_index] = 9;
  }
  else if (chip8_machine->keypad_state[10])
  {
    chip8_machine->registers[x_index] = 10;
  }
  else if (chip8_machine->keypad_state[11])
  {
    chip8_machine->registers[x_index] = 11;
  }
  else if (chip8_machine->keypad_state[12])
  {
    chip8_machine->registers[x_index] = 12;
  }
  else if (chip8_machine->keypad_state[13])
  {
    chip8_machine->registers[x_index] = 13;
  }
  else if (chip8_machine->keypad_state[14])
  {
    chip8_machine->registers[x_index] = 14;
  }
  else if (chip8_machine->keypad_state[15])
  {
    chip8_machine->registers[x_index] = 15;
  }
  else
  {
    // @note considerando que o 'PC' foi incrementado antes de chamar essa
    // função, o que é o comportamento atual da função `execute_a_cycle`,
    // o ato de decrementar o 'PC' aqui fará com que não haja progresso e a instrução
    // de 'wait key' seja executada novamente, isso até haver algum input.
    // Acho que seria interessante retornar algum sinal da função `execute_a_cycle`
    // para indicar a espera de input e organizar o código para simplesmente não
    // executar `execute_a_cycle` até ter algum input. Isso salvaria ciclos da CPU
    chip8_machine->program_counter -= 2;
  }
  
}

/**
 * @brief Load DT, Vx - Set delay timer = Vx
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx15(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  chip8_machine->delay_timer = chip8_machine->registers[x_index];
}

/**
 * @brief Load ST, Vx - Set sound timer = Vx
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx18(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  chip8_machine->sound_timer = chip8_machine->registers[x_index];
}

/**
 * @brief Add I, Vx - Set I = I + Vx
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx1E(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  chip8_machine->index_register += chip8_machine->registers[x_index];
}

/**
 * @brief Load Font, Vx - Set I = location of sprite for digit Vx
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx29(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t digit_value = chip8_machine->registers[x_index];
  assert(digit_value < 17); // @todo João, avaliar se precisa tratar

  chip8_machine->index_register = FONT_START_ADDRESS + (5 * digit_value);
}

/**
 * @brief Load BCD, Vx - Store BCD representation of Vx in memory locations I, I+1, I+2 
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx33(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;
  uint8_t value = chip8_machine->registers[x_index];
  
  // unidade
  chip8_machine->memory[chip8_machine->index_register + 2] = value % 10;

  // decimal
  value /= 10;  
  chip8_machine->memory[chip8_machine->index_register + 1] = value % 10;
  
  // centena
  value /= 10;
  chip8_machine->memory[chip8_machine->index_register + 0] = value % 10;
}

/**
 * @brief Load [I], Vx
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx55(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  for (uint8_t i_index = 0; i_index <= x_index; i_index++)
  {
    chip8_machine->memory[chip8_machine->index_register + i_index] = chip8_machine->registers[i_index];
  }
}

/**
 * @brief Load Vx, [I] - Read registers V0 through Vx in memory starting at location I
 * 
 * @param chip8_machine 
 * @param opcode 
 */
void execute_op_Fx65(Chip8_Machine *chip8_machine, uint16_t opcode)
{
  uint8_t x_index = (opcode & 0x0F00u) >> 8u;

  for (uint8_t i_index = 0; i_index <= x_index; i_index++)
  {
    chip8_machine->registers[i_index] = chip8_machine->memory[chip8_machine->index_register + i_index];
  }
}
