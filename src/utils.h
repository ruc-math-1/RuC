#ifndef _UTILS_H
#define _UTILS_H 1

#include "stdio.h"

void printf_char(char wchar)
{
  printf("%c", wchar);
}

void fprintf_char(FILE f, int wchar)
{
  fputc(wchar, f);
}

char getf_char()
{
  return getc();
}

#endif
