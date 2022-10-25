/*
 *  Copyright 2022 Andrey Terekhov, Mikhail Terekhov
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#define EOF -1

void printf_char(char wchar)
{
  printf("%c", wchar);
}

// void fprintf_char(FILE *f, int wchar)
// {
//   if (wchar < 128)
//   {
//     fprintf(f, "%c", wchar);
//   }
//   else
//   {
//     unsigned char first = (wchar >> 6) | /*0b11000000*/ 0xC0;
//     unsigned char second = (wchar & /*0b111111*/ 0x3F) | /*0b10000000*/ 0x80;

//     fprintf(f, "%c%c", first, second);
//   }
// }

/*char getf_char()
{
  char wchar;

  if (scanf("%c", &wchar) == EOF)
  {
    return EOF;
  }
  return wchar;
}*/
