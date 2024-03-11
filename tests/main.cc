#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../src/chip8_machine.cc"

void test_op_00E0(void)
{
  Chip8_Machine machine = {};

  machine.memory[0] = 32;
  execute_op_00E0(&machine, 0x00E0);
  assert(machine.memory[0] == 0);
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
  test_op_00E0();

  return EXIT_SUCCESS;
}