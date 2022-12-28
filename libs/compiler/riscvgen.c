/*
 *  Copyright 2021 Andrey Terekhov, Ivan S. Arkhipov
 *  and Georgiy Belyanin, Timofey Florov, Ignat Sergeev
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

#include "riscvgen.h"
#include <string.h>
#include "AST.h"
#include "hash.h"
#include "operations.h"
#include "tree.h"
#include "uniprinter.h"

typedef enum RISCV_REGISTER
{
  R_ZERO = 0, /**< Always has the value 0 */

  R_RA = 1,
  R_SP = 2,
  R_GP = 3,
  R_TP = 4,

  R_T0 = 5,
  R_T1 = 6,
  R_T2 = 7,

  R_FP = 8,
  R_S0 = 8,

  R_S1 = 9,
  R_A0 = 10,
  R_A1 = 11,
  R_A2 = 12,
  R_A3 = 13,
  R_A4 = 14,
  R_A5 = 15,
  R_A6 = 16,
  R_A7 = 17,

  R_S2 = 18,
  R_S3 = 19,
  R_S4 = 20,
  R_S5 = 21,
  R_S6 = 22,
  R_S7 = 23,
  R_S8 = 24,
  R_S9 = 25,
  R_S10 = 26,
  R_S11 = 27,

  R_T3 = 28,
  R_T4 = 29,
  R_T5 = 30,
  R_T6 = 31,
} riscv_register_t;