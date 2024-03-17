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

void test_op_1nnn(void)
{
  Chip8_Machine machine = {};

  machine.program_counter = 0;
  execute_op_1nnn(&machine, 0x10FFu);
  assert(machine.program_counter == 0x0FFu);

  machine.program_counter = 0;
  execute_op_1nnn(&machine, 0x1AF3u);
  assert(machine.program_counter == 0xAF3u);
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
  uint16_t opcode = 0x5350;

  machine.registers[3] = 0xAE;
  machine.registers[5] = 0xAC;
  machine.program_counter = 0;

  execute_op_5xy0(&machine, opcode);
  assert(machine.program_counter == 0);

  machine.registers[3] = 0xAE;
  machine.registers[5] = 0xAE;
  machine.program_counter = 0;

  execute_op_5xy0(&machine, opcode);
  assert(machine.program_counter == 2);
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

void test_op_7xkk(void)
{
  Chip8_Machine machine = {};

  machine.registers[0xA] = 1;
  execute_op_7xkk(&machine, 0x7AAB);
  assert(machine.registers[0xA] == 0xAC);

  machine.registers[0x6] = 1;
  execute_op_7xkk(&machine, 0x76AB);
  assert(machine.registers[0x6] == 0xAC);
}

void test_op_8xy0(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x2] = 0;
  machine.registers[0x4] = 6;
  machine.registers[0x5] = 3;

  execute_op_8xy0(&machine, 0x8240);
  assert(machine.registers[0x2] == 6);

  execute_op_8xy0(&machine, 0x8450);
  assert(machine.registers[0x4] == 3);
}

void test_op_8xy1(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x2] = 0x0F;
  machine.registers[0x4] = 0xF0;

  execute_op_8xy1(&machine, 0x8240);
  assert(machine.registers[0x2] == 0xFF);

  machine.registers[0x5] = 22;
  machine.registers[0x7] = 45;

  execute_op_8xy1(&machine, 0x8570);
  assert(machine.registers[0x5] == (22 | 45));
}

void test_op_9xkk(void)
{
  Chip8_Machine machine = {};
  
  uint16_t opcode = 0x5350;

  machine.registers[3] = 0xAE;
  machine.registers[5] = 0xAC;
  machine.program_counter = 0;

  execute_op_9xy0(&machine, opcode);
  assert(machine.program_counter == 2);

  machine.registers[3] = 0xAE;
  machine.registers[5] = 0xAE;
  machine.program_counter = 0;

  execute_op_9xy0(&machine, opcode);
  assert(machine.program_counter == 0);
}

void test_op_Annn(void)
{
  Chip8_Machine machine = {};
  uint16_t opcode = 0xA333;

  machine.index_register = 0;

  execute_op_Annn(&machine, opcode);
  assert(machine.index_register == 0x333);
}

void test_op_Bnnn(void)
{
  Chip8_Machine machine = {};

  machine.program_counter = 0;
  machine.registers[0] = 0;

  execute_op_Bnnn(&machine, 0xB123);
  assert(machine.program_counter == 0x0123);

  execute_op_Bnnn(&machine, 0xB125);
  assert(machine.program_counter == 0x0125);
}


void test_op_Cxkk(void)
{
  Chip8_Machine machine = {};

  machine.registers[2] = 5;
  execute_op_Cxkk(&machine, 0xC200);
  assert(machine.registers[2] == 0);

  machine.registers[5] = 5;
  execute_op_Cxkk(&machine, 0xC500);
  assert(machine.registers[5] == 0);

  for (int i = 0; i < 128; i++)
  {
    machine.registers[2] = UINT8_MAX;
    execute_op_Cxkk(&machine, 0xC20F);
    assert(machine.registers[2] <= 0x0F);

    machine.registers[2] = UINT8_MAX;
    execute_op_Cxkk(&machine, 0xC21F);
    assert(machine.registers[2] <= 0x1F);
  }
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
  test_op_1nnn();
  test_op_3xkk();
  test_op_4xkk();
  test_op_5xy0();
  test_op_6xkk();
  test_op_7xkk();
  test_op_8xy0();
  test_op_8xy1();
  test_op_9xkk();
  test_op_Annn();
  test_op_Bnnn();
  test_op_Cxkk();

  return EXIT_SUCCESS;
}