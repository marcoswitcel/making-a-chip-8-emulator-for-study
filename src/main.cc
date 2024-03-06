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

  // Agora seria interessante fazer um esboço inicial de como vou separar
  // a emulação do código que faz a exibição do output, pra deixar mais abstraído da plataforma.

  return open_window();
}
