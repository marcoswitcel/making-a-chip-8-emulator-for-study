#include <stdio.h>
#include <stdlib.h>

#include "chip8_machine.cc"
#include "window.cc"
#include "utils.macro.h"

int main(int argc, char **argv)
{
  UNUSED(argc);
  UNUSED(argv);

  printf("Olá mundo da emulação Chip-8!\n");

  if (argc < 2)
  {
    printf("Nome do arquivo a ser executado faltando...\n");
    printf("Exemplo de uso:\nmain test_opcode.ch8\n");
    return EXIT_FAILURE;
  }

  // Agora seria interessante fazer um esboço inicial de como vou separar
  // a emulação do código que faz a exibição do output, pra deixar mais abstraído da plataforma.

  return open_window(argv[1]);
}
