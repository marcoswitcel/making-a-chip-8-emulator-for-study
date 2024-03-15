#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../src/chip8_machine.cc"

#include "../src/utils.macro.h"

void test_op_00E0(void)
{
  Chip8_Machine machine = {};

  machine.memory[0] = 32;
  execute_op_00E0(&machine, 0x00E0);
  assert(machine.memory[0] == 0);
}

void test_op_3xkk(void)
{
  Chip8_Machine machine = {};
  
  {
    uint16_t opcode = 0x33AE;

    machine.registers[3] = 0xAE;
    machine.program_counter = 0;

    execute_op_3xkk(&machine, opcode);
    assert(machine.program_counter == 2);
  }

  {
    uint16_t opcode = 0x33AE;

    machine.registers[3] = 0x00;
    machine.program_counter = 0;

    execute_op_3xkk(&machine, opcode);
    assert(machine.program_counter == 0);
  }
}

void test_op_4xkk(void)
{
  Chip8_Machine machine = {};
  
  {
    uint16_t opcode = 0x33AE;

    machine.registers[3] = 0xAE;
    machine.program_counter = 0;

    execute_op_4xkk(&machine, opcode);
    assert(machine.program_counter == 0);
  }

  {
    uint16_t opcode = 0x33AE;

    machine.registers[3] = 0x00;
    machine.program_counter = 0;

    execute_op_4xkk(&machine, opcode);
    assert(machine.program_counter == 2);
  }
}

void test_op_5xy0(void)
{
  Chip8_Machine machine = {};
  
  {
    uint16_t opcode = 0x5350;

    machine.registers[3] = 0xAE;
    machine.registers[5] = 0xAC;
    machine.program_counter = 0;

    execute_op_5xy0(&machine, opcode);
    assert(machine.program_counter == 0);
  }

  {
    uint16_t opcode = 0x5350;

    machine.registers[3] = 0xAE;
    machine.registers[5] = 0xAE;
    machine.program_counter = 0;

    execute_op_5xy0(&machine, opcode);
    assert(machine.program_counter == 2);
  }
}

void test_op_6xkk(void)
{
  Chip8_Machine machine = {};

  machine.registers[0xA] = 0;
  execute_op_6xkk(&machine, 0x6ABE);
  assert(machine.registers[0xA] == 0xBE);

  machine.registers[0x6] = 0;
  execute_op_6xkk(&machine, 0x66BE);
  assert(machine.registers[0x6] == 0xBE);
}

/**
 * @brief ponto de entrada dos testes
 * @todo João, considerar trazer a interface gráfica e as melhorias dos outros projetos para esse
 * 'test runner' aqui.
 * 
 * @return int 
 */
int main(void)
{
  UNUSED(load_rom);

  test_op_00E0();
  test_op_3xkk();
  test_op_4xkk();
  test_op_5xy0();
  test_op_6xkk();

  return EXIT_SUCCESS;
}