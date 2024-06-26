#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void test_op_00EE(void)
{
  Chip8_Machine machine = {};

  machine.last_opcode_signal = NONE;

  machine.program_counter = 0x0055;
  machine.stack[0] = 0x0032;
  machine.stack_pointer = 1;
  execute_op_00EE(&machine, 0x00EE);
  assert(machine.program_counter == 0x0032);
  assert(machine.stack_pointer == 0);
  assert(machine.last_opcode_signal == NONE);

  // sinal emitido em caso de underflow da stack
  execute_op_00EE(&machine, 0x00EE);
  assert(machine.last_opcode_signal == STACK_UNDERFLOW);
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

void test_op_2nnn(void)
{
  Chip8_Machine machine = {};

  machine.last_opcode_signal = NONE;

  machine.program_counter = 0xCE;
  machine.stack_pointer = 0;
  machine.stack[0] = 0;
  execute_op_2nnn(&machine, 0x20FFu);
  assert(machine.program_counter == 0x0FFu);
  assert(machine.stack_pointer == 1);
  assert(machine.stack[0] == 0xCE);
  assert(machine.last_opcode_signal == NONE);

  machine.program_counter = 0xCE;
  machine.stack_pointer = 0;
  machine.stack[0] = 0;
  execute_op_2nnn(&machine, 0x2AF3u);
  assert(machine.program_counter == 0xAF3u);
  assert(machine.stack_pointer == 1);
  assert(machine.stack[0] == 0xCE);
  assert(machine.last_opcode_signal == NONE);

  // signal emitido em caso de overflow da stack
  machine.program_counter = 0x00CC;
  machine.stack_pointer = 0;
  machine.stack[0] = 0;

  uint16_t last_pc;

  for (unsigned i  = 0; i < CHIP8_STACK_SIZE; i++)
  {
    last_pc = machine.program_counter;

    // n chamada
    execute_op_2nnn(&machine, 0x20C1);
    assert(machine.program_counter == 0x00C1);
    assert(machine.stack_pointer == (i + 1));
    assert(machine.stack[i] == last_pc);
    assert(machine.last_opcode_signal == NONE);
  }

  // CHIP8_STACK_SIZEª chamada
  execute_op_2nnn(&machine, 0x20C1);
  assert(machine.program_counter == 0x00C1);
  assert(machine.stack_pointer == 16);
  assert(machine.stack[CHIP8_STACK_SIZE - 1] == last_pc);
  assert(machine.last_opcode_signal == STACK_OVERFLOW);

  // CHIP8_STACK_SIZE + 1 ª chamada
  execute_op_2nnn(&machine, 0x20C1);
  assert(machine.program_counter == 0x00C1);
  assert(machine.stack_pointer == 16);
  assert(machine.stack[CHIP8_STACK_SIZE - 1] == last_pc);
  assert(machine.last_opcode_signal == STACK_OVERFLOW);
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

void test_op_8xy2(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x2] = 0x0F;
  machine.registers[0x4] = 0xF0;

  execute_op_8xy2(&machine, 0x8240);
  assert(machine.registers[0x2] == 0x00);

  machine.registers[0x5] = 22;
  machine.registers[0x7] = 45;

  execute_op_8xy2(&machine, 0x8570);
  assert(machine.registers[0x5] == (22 & 45));
}

void test_op_8xy3(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x2] = 0x0F;
  machine.registers[0x4] = 0xF0;

  execute_op_8xy3(&machine, 0x8240);
  assert(machine.registers[0x2] == 0xFF);

  machine.registers[0x2] = 0x0F;
  machine.registers[0x4] = 0xFF;

  execute_op_8xy3(&machine, 0x8240);
  assert(machine.registers[0x2] == 0xF0);

  machine.registers[0x2] = 0xFF;
  machine.registers[0x4] = 0xFF;

  execute_op_8xy3(&machine, 0x8240);
  assert(machine.registers[0x2] == 0x00);

  machine.registers[0x5] = 22;
  machine.registers[0x7] = 45;

  execute_op_8xy3(&machine, 0x8570);
  assert(machine.registers[0x5] == (22 ^ 45));
}

void test_op_8xy4(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x2] = 5;
  machine.registers[0x4] = 6;
  execute_op_8xy4(&machine, 0x8244);
  assert(machine.registers[0x2] == 11);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x3] = UINT8_MAX;
  machine.registers[0x8] = 55;
  execute_op_8xy4(&machine, 0x8384);
  assert(machine.registers[0x3] == ((UINT8_MAX + 55) & 0xFF));
  assert(machine.registers[0xF] == 1);

  machine.registers[0x3] = 233;
  machine.registers[0x8] = 55;
  execute_op_8xy4(&machine, 0x8384);
  assert(machine.registers[0x3] == ((233 + 55) & 0xFF));
  assert(machine.registers[0xF] == 1);
}

void test_op_8xy5(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x2] = 5;
  machine.registers[0x4] = 6;
  execute_op_8xy5(&machine, 0x8245);
  /**
   * @note Não é dito claramente, mas quando se subtrai dois uint8_t acredito
   * que se espere o comportamento de 'wrap-around'
   */
  assert(machine.registers[0x2] == UINT8_MAX);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x3] = UINT8_MAX;
  machine.registers[0x8] = 55;
  execute_op_8xy5(&machine, 0x8385);
  assert(machine.registers[0x3] == (UINT8_MAX - 55));
  assert(machine.registers[0xF] == 1);

  machine.registers[0x3] = 233;
  machine.registers[0x8] = 55;
  execute_op_8xy5(&machine, 0x8385);
  assert(machine.registers[0x3] == (233 - 55));
  assert(machine.registers[0xF] == 1);
}

void test_op_8xy6(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x5] = 4;
  execute_op_8xy6(&machine, 0x8556);
  assert(machine.registers[0x5] == 2);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x5] = 33;
  execute_op_8xy6(&machine, 0x8556);
  assert(machine.registers[0x5] == 16);
  assert(machine.registers[0xF] == 1);

  machine.registers[0x5] = 32;
  execute_op_8xy6(&machine, 0x8556);
  assert(machine.registers[0x5] == 16);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x5] = 35;
  execute_op_8xy6(&machine, 0x8556);
  assert(machine.registers[0x5] == 17);
  assert(machine.registers[0xF] == 1);
}

void test_op_8xy7(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x2] = 6;
  machine.registers[0x4] = 5;
  execute_op_8xy7(&machine, 0x8247);
  /**
   * @note Não é dito claramente, mas quando se subtrai dois uint8_t acredito
   * que se espere o comportamento de 'wrap-around'
   */
  assert(machine.registers[0x2] == UINT8_MAX);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x3] = 55;
  machine.registers[0x8] = UINT8_MAX;
  execute_op_8xy7(&machine, 0x8387);
  assert(machine.registers[0x3] == (UINT8_MAX - 55));
  assert(machine.registers[0xF] == 1);

  machine.registers[0x3] = 55;
  machine.registers[0x8] = 233;
  execute_op_8xy7(&machine, 0x8387);
  assert(machine.registers[0x3] == (233 - 55));
  assert(machine.registers[0xF] == 1);
}

void test_op_8xyE(void)
{
  Chip8_Machine machine = {};

  machine.registers[0x5] = 4;
  execute_op_8xyE(&machine, 0x855E);
  assert(machine.registers[0x5] == 8);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x5] = 33;
  execute_op_8xyE(&machine, 0x855E);
  assert(machine.registers[0x5] == 66);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x5] = 32;
  execute_op_8xyE(&machine, 0x855E);
  assert(machine.registers[0x5] == 64);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x5] = 35;
  execute_op_8xyE(&machine, 0x855E);
  assert(machine.registers[0x5] == 70);
  assert(machine.registers[0xF] == 0);

  machine.registers[0x5] = 0x80;
  execute_op_8xyE(&machine, 0x855E);
  assert(machine.registers[0x5] == 0);
  assert(machine.registers[0xF] == 1);

  machine.registers[0x5] = 0xC0;
  execute_op_8xyE(&machine, 0x855E);
  assert(machine.registers[0x5] == 0x80);
  assert(machine.registers[0xF] == 1);
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

void test_op_Ex9E(void)
{
  Chip8_Machine machine = {};

  machine.program_counter = 0;
  machine.registers[2] = 5;
  machine.keypad_state[5] = true;
  execute_op_Ex9E(&machine, 0xE29E);
  assert(machine.program_counter == 2);

  machine.program_counter = 0;
  machine.registers[2] = 5;
  machine.keypad_state[5] = false;
  execute_op_Ex9E(&machine, 0xE29E);
  assert(machine.program_counter == 0);
}

void test_op_ExA1(void)
{
  Chip8_Machine machine = {};

  machine.program_counter = 0;
  machine.registers[2] = 5;
  machine.keypad_state[5] = true;
  execute_op_ExA1(&machine, 0xE2A1);
  assert(machine.program_counter == 0);

  machine.program_counter = 0;
  machine.registers[2] = 5;
  machine.keypad_state[5] = false;
  execute_op_ExA1(&machine, 0xE2A1);
  assert(machine.program_counter == 2);
}

void test_op_Fx07(void)
{
  Chip8_Machine machine = {};

  machine.delay_timer = 55;
  machine.registers[3] = 0;
  execute_op_Fx07(&machine, 0xF307);
  assert(machine.registers[3] == 55);

  machine.delay_timer = 28;
  machine.registers[5] = 0;
  execute_op_Fx07(&machine, 0xF507);
  assert(machine.registers[5] == 28);
}

void test_op_Fx0A(void)
{
  Chip8_Machine machine = {};

  machine.program_counter = 2;
  machine.registers[3] = 0;
  memset(machine.keypad_state, 0, sizeof(uint8_t) * 8);
  execute_op_Fx0A(&machine, 0xF30A);
  assert(machine.program_counter == 0);
  assert(machine.registers[3] == 0);

  machine.program_counter = 2;
  machine.registers[4] = 0;
  memset(machine.keypad_state, 0, sizeof(uint8_t) * 8);
  machine.keypad_state[5] = true;
  execute_op_Fx0A(&machine, 0xF40A);
  assert(machine.program_counter == 2);
  assert(machine.registers[4] == 5);

  machine.program_counter = 4;
  machine.registers[6] = 0;
  memset(machine.keypad_state, 0, sizeof(uint8_t) * 8);
  machine.keypad_state[7] = true;
  execute_op_Fx0A(&machine, 0xF60A);
  assert(machine.program_counter == 4);
  assert(machine.registers[6] == 7);
}

void test_op_Fx15(void)
{
  Chip8_Machine machine = {};

  machine.delay_timer = 0;
  machine.registers[3] = 55;
  execute_op_Fx15(&machine, 0xF315);
  assert(machine.delay_timer == 55);

  machine.delay_timer = 0;
  machine.registers[5] = 28;
  execute_op_Fx15(&machine, 0xF515);
  assert(machine.delay_timer == 28);
}

void test_op_Fx18(void)
{
  Chip8_Machine machine = {};

  machine.sound_timer = 0;
  machine.registers[3] = 55;
  execute_op_Fx18(&machine, 0xF318);
  assert(machine.sound_timer == 55);

  machine.sound_timer = 0;
  machine.registers[5] = 28;
  execute_op_Fx18(&machine, 0xF518);
  assert(machine.sound_timer == 28);
}

void test_op_Fx1E(void)
{
  Chip8_Machine machine = {};

  machine.index_register = 0;
  machine.registers[3] = 55;
  execute_op_Fx1E(&machine, 0xF31E);
  assert(machine.index_register == 55);

  machine.index_register = 3;
  machine.registers[3] = 55;
  execute_op_Fx1E(&machine, 0xF31E);
  assert(machine.index_register == 58);
}

void test_op_Fx29(void)
{
  Chip8_Machine machine = {};

  machine.index_register = 0;
  machine.registers[3] = 0;
  execute_op_Fx29(&machine, 0xF329);
  assert(machine.index_register == FONT_START_ADDRESS);

  machine.index_register = 0;
  machine.registers[3] = 1;
  execute_op_Fx29(&machine, 0xF329);
  assert(machine.index_register == (FONT_START_ADDRESS + 5));

  machine.index_register = 0;
  machine.registers[4] = 4;
  execute_op_Fx29(&machine, 0xF429);
  assert(machine.index_register == (FONT_START_ADDRESS + 20));
}

void test_op_Fx33(void)
{
  Chip8_Machine machine = {};

  machine.index_register = 0;
  machine.registers[3] = 251;
  execute_op_Fx33(&machine, 0xF333);
  assert(machine.memory[machine.index_register + 0] == 2);
  assert(machine.memory[machine.index_register + 1] == 5);
  assert(machine.memory[machine.index_register + 2] == 1);

  machine.index_register = 10;
  machine.registers[5] = 251;
  execute_op_Fx33(&machine, 0xF533);
  assert(machine.memory[machine.index_register + 0] == 2);
  assert(machine.memory[machine.index_register + 1] == 5);
  assert(machine.memory[machine.index_register + 2] == 1);

  machine.index_register = 0;
  machine.registers[3] = 134;
  execute_op_Fx33(&machine, 0xF333);
  assert(machine.memory[machine.index_register + 0] == 1);
  assert(machine.memory[machine.index_register + 1] == 3);
  assert(machine.memory[machine.index_register + 2] == 4);

  machine.index_register = 10;
  machine.registers[5] = 134;
  execute_op_Fx33(&machine, 0xF533);
  assert(machine.memory[machine.index_register + 0] == 1);
  assert(machine.memory[machine.index_register + 1] == 3);
  assert(machine.memory[machine.index_register + 2] == 4);
}

void test_op_Fx55(void)
{
  Chip8_Machine machine = {};

  machine.index_register = 0;
  
  machine.memory[machine.index_register + 0] = 0;
  machine.memory[machine.index_register + 1] = 0;
  machine.memory[machine.index_register + 2] = 0;
  machine.memory[machine.index_register + 4] = 0;
  machine.memory[machine.index_register + 5] = 0;
  machine.memory[machine.index_register + 6] = 0;

  machine.registers[0] = 37;
  machine.registers[1] = 37;
  machine.registers[2] = 37;
  machine.registers[4] = 37;
  machine.registers[5] = 37;
  machine.registers[6] = 37;

  execute_op_Fx55(&machine, 0xF555);
  assert(machine.memory[machine.index_register + 0] == 37);
  assert(machine.memory[machine.index_register + 1] == 37);
  assert(machine.memory[machine.index_register + 2] == 37);
  assert(machine.memory[machine.index_register + 4] == 37);
  assert(machine.memory[machine.index_register + 5] == 37);
  assert(machine.memory[machine.index_register + 6] == 0);

  // index alterado
  machine.index_register = 21;
  
  machine.memory[machine.index_register + 0] = 0;
  machine.memory[machine.index_register + 1] = 0;
  machine.memory[machine.index_register + 2] = 0;
  machine.memory[machine.index_register + 4] = 0;
  machine.memory[machine.index_register + 5] = 0;
  machine.memory[machine.index_register + 6] = 0;

  machine.registers[0] = 38;
  machine.registers[1] = 38;
  machine.registers[2] = 38;
  machine.registers[4] = 38;
  machine.registers[5] = 38;
  machine.registers[6] = 38;

  execute_op_Fx55(&machine, 0xF455);
  assert(machine.memory[machine.index_register + 0] == 38);
  assert(machine.memory[machine.index_register + 1] == 38);
  assert(machine.memory[machine.index_register + 2] == 38);
  assert(machine.memory[machine.index_register + 4] == 38);
  assert(machine.memory[machine.index_register + 5] == 0);
  assert(machine.memory[machine.index_register + 6] == 0);
}

void test_op_Fx65(void)
{
  Chip8_Machine machine = {};

  machine.index_register = 0;

  machine.registers[0] = 0;
  machine.registers[1] = 0;
  machine.registers[2] = 0;
  machine.registers[4] = 0;
  machine.registers[5] = 0;
  machine.registers[6] = 0;
  
  machine.memory[machine.index_register + 0] = 37;
  machine.memory[machine.index_register + 1] = 37;
  machine.memory[machine.index_register + 2] = 37;
  machine.memory[machine.index_register + 4] = 37;
  machine.memory[machine.index_register + 5] = 37;
  machine.memory[machine.index_register + 6] = 37;

  execute_op_Fx65(&machine, 0xF565);
  assert(machine.registers[0] == 37);
  assert(machine.registers[1] == 37);
  assert(machine.registers[2] == 37);
  assert(machine.registers[4] == 37);
  assert(machine.registers[5] == 37);
  assert(machine.registers[6] == 0);

  // index alterado
  machine.index_register = 21;

  machine.registers[0] = 0;
  machine.registers[1] = 0;
  machine.registers[2] = 0;
  machine.registers[4] = 0;
  machine.registers[5] = 0;
  machine.registers[6] = 0;
  
  machine.memory[machine.index_register + 0] = 38;
  machine.memory[machine.index_register + 1] = 38;
  machine.memory[machine.index_register + 2] = 38;
  machine.memory[machine.index_register + 4] = 38;
  machine.memory[machine.index_register + 5] = 38;
  machine.memory[machine.index_register + 6] = 38;

  execute_op_Fx65(&machine, 0xF465);
  assert(machine.registers[0] == 38);
  assert(machine.registers[1] == 38);
  assert(machine.registers[2] == 38);
  assert(machine.registers[4] == 38);
  assert(machine.registers[5] == 0);
  assert(machine.registers[6] == 0);
}

void test_call_ret_behavior(void)
{
  Chip8_Machine machine = {};

  uint16_t intial_pc = 0x00A0u;

  machine.program_counter = intial_pc;
  machine.stack_pointer = 0;
  machine.stack[0] = 0;
  execute_op_2nnn(&machine, 0x20BCu);
  assert(machine.program_counter == 0x0BCu);
  assert(machine.stack_pointer == 1);
  assert(machine.stack[0] == intial_pc);

  execute_op_00EE(&machine, 0x00EEu);
  assert(machine.program_counter == intial_pc);
  assert(machine.stack_pointer == 0);
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
  test_op_00EE();
  test_op_1nnn();
  test_op_2nnn();
  test_op_3xkk();
  test_op_4xkk();
  test_op_5xy0();
  test_op_6xkk();
  test_op_7xkk();
  test_op_8xy0();
  test_op_8xy1();
  test_op_8xy2();
  test_op_8xy3();
  test_op_8xy4();
  test_op_8xy5();
  test_op_8xy6();
  test_op_8xy7();
  test_op_8xyE();
  test_op_9xkk();
  test_op_Annn();
  test_op_Bnnn();
  test_op_Cxkk();
  test_op_Ex9E();
  test_op_ExA1();
  test_op_Fx07();
  test_op_Fx0A();
  test_op_Fx15();
  test_op_Fx18();
  test_op_Fx1E();
  test_op_Fx29();
  test_op_Fx33();
  test_op_Fx55();
  test_op_Fx65();

  // cheques de comportamentos específicos
  test_call_ret_behavior();
  
  printf("Sucesso! 100%% dos testes passaram.");

  return EXIT_SUCCESS;
}