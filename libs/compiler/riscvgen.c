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

typedef enum RISCV_EXTENSION 
{
	E_RVWMO,
	E_RV32I,
	E_RV32E,
	E_RV64I,
	E_RV128I,

	E_M,
	E_A,
	E_F,
	E_D,
	E_Zicr,
	E_Zifencei,
	E_G,
	E_Q,
	E_L,
	E_C,
	E_B,
	E_J,
	E_T,
	E_P,
	E_K,
	E_N,
	E_H,
	E_S,

	E_Zam,
	E_Ztso
} riscv_extension_t;

typedef enum RISCV_REGISTER
{
	R_ZERO, /**< Always has the value 0 */

	R_RA,
	R_SP,
	R_GP,
	R_TP,

	R_T0,
	R_T1,
	R_T2,

	R_FP = 8,
	R_S0 = 8,
	R_S1,

	R_A0,
	R_A1,
	R_A2,
	R_A3,
	R_A4,
	R_A5,
	R_A6,
	R_A7,

	R_S2,
	R_S3,
	R_S4,
	R_S5,
	R_S6,
	R_S7,
	R_S8,
	R_S9,
	R_S10,
	R_S11,

	R_T3,
	R_T4,
	R_T5,
	R_T6,

	R_FLOATING_POINT_REGISTERS = 32,
	R_FT0,
	R_FT1,
	R_FT2,
	R_FT3,
	R_FT4,
	R_FT5,
	R_FT6,
	R_FT7,

	R_FS0,
	R_FS1,

	R_FA0,
	R_FA1,
	R_FA2,
	R_FA3,
	R_FA4,
	R_FA5,
	R_FA6,
	R_FA7,

	R_FS2,
	R_FS3,
	R_FS4,
	R_FS5,
	R_FS6,
	R_FS7,
	R_FS8,
	R_FS9,
	R_FS10,
	R_FS11,

	R_FT8,
	R_FT9,
	R_FT10,
	R_FT11
} riscv_register_t;

typedef enum RISCV_INSTRUCTION
{
	IC_LUI,
	IC_AUIPC,
	IC_JAL,
	IC_JALR,
	IC_BEQ,
	IC_BNE,
	IC_BLT,
	IC_BGE,
	IC_BLTU,
	IC_BGEU,
	IC_LB,
	IC_LH,
	IC_LW,
	IC_LBU,
	IC_LHU,
	IC_SB,
	IC_SH,
	IC_SW,
	IC_ADDI,
	IC_SLTI,
	IC_SLTIU,
	IC_XORI,
	IC_ORI,
	IC_ANDI,
	IC_SLLI,
	IC_SRLI,
	IC_SRAI,
	IC_ADD,
	IC_SUB,
	IC_SLL,
	IC_SLT,
	IC_SLTU,
	IC_XOR,
	IC_SRL,
	IC_SRA,
	IC_OR,
	IC_AND,
	IC_FENCE,
	IC_FENCEI,
} riscv_instruction_t;
