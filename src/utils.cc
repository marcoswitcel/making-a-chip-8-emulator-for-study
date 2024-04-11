#pragma once

#include <stdlib.h>

/**
 * @brief Gera um string com a representação textual do inteiro
 * 
 * @note João, não seria complexo adicionar uma versão que suporte bases
 * alternativas como base: 2, 16 e etc... Pode-se usar o código e os links de
 * referência abaixo. 
 * 
 * Referências
 * @link https://daveparillo.github.io/cisc187-reader/recursion/to_string.html
 * @link https://panda.ime.usp.br/panda/static/pythonds_pt/04-Recursao/04-int2str.html
 * 
 * @param number 
 * @return char* 
 */
char *int_to_cstring(int number)
{
  const int base = 10;
  bool is_negative_number = number < 0;
  int n_chars = 1 + is_negative_number;
  int copy = number;
  // @note a inversão do sinal poderia ser feita com uma negação e uma adição "copy = ~copy + 1;"
  if (is_negative_number) copy *= -1;
  
  while ((copy /= 10) > 0) n_chars++;

  char *number_cstring = (char *) malloc(n_chars + 1);

  int currentNumber = number;
  if (is_negative_number) currentNumber *= -1;
  
  for (int i = n_chars; i--;)
  {
    if (currentNumber < base)
    {
      number_cstring[i] = '0' + currentNumber;
      break;
    }

    int remainder = currentNumber % base;
    number_cstring[i] = '0' + remainder;
    currentNumber /= base;
  }
  
  if (is_negative_number)
  {
    number_cstring[0] = '-';
  }
  number_cstring[n_chars] = '\0';

  return number_cstring;
}

typedef enum Numerical_Base {
  BINARY = 2,
  OCTAL =  4,
  DECIMAL = 10,
  HEXADECIMAL = 16,
} Numerical_Base;

char *int_to_cstring_in_base(int number, const Numerical_Base base)
{
  static const char symbols[] = "0123456789ABCDEF";

  bool is_negative_number = number < 0;
  int n_chars = 1 + is_negative_number;
  int copy = number;
  // @note a inversão do sinal poderia ser feita com uma negação e uma adição "copy = ~copy + 1;"
  if (is_negative_number) copy *= -1;
  
  while ((copy /= base) > 0) n_chars++;

  char *number_cstring = (char *) malloc(n_chars + 1);

  int currentNumber = number;
  if (is_negative_number) currentNumber *= -1;
  
  for (int i = n_chars; i--;)
  {
    if (currentNumber < (int) base)
    {
      number_cstring[i] = symbols[currentNumber];
      break;
    }

    int remainder = currentNumber % base;
    number_cstring[i] = symbols[remainder];
    currentNumber /= base;
  }
  
  if (is_negative_number)
  {
    number_cstring[0] = '-';
  }
  number_cstring[n_chars] = '\0';

  return number_cstring;
}
