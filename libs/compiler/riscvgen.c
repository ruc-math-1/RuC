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

	IC_RV64I_INSTRUCTIONS,
	IC_LWU,
	IC_ADDIW,
	IC_SLLIW,
	IC_SRLIW,
	IC_SRAIW,
	IC_ADDW,
	IC_SUBW,
	IC_SLLW,
	IC_SRLW,
	IC_SRAW,

	IC_M32_INSTRUCTIONS,
	IC_MUL,
	IC_MULH,
	IC_MULHSU, 
	IC_MULHU,
	IC_DIV,
	IC_DIVU,
	IC_REM,
	IC_REMU,

	IC_M64_INSTRUCTIONS,
	IC_MULW,
	IC_DIVW,
	IC_DIVUW,
	IC_REMW,
	IC_REMUW

} riscv_instruction_t;

static void riscv_register_to_io(universal_io *const io, const mips_register_t reg)
{
	switch (reg)
	{
		case R_ZERO:
			uni_printf(io, "zero");
			break;

		case R_RA:
			uni_printf(io, "ra");
			break;
		case R_SP:
			uni_printf(io, "sp");
			break;
		case R_GP:
			uni_printf(io, "gp");
			break;
		case R_TP:
			uni_printf(io, "tp");
			break;

		case R_T0:
			uni_printf(io, "t0");
			break;
		case R_T1:
			uni_printf(io, "t1");
			break;
		case R_T2:
			uni_printf(io, "t2");
			break;

		case R_FP:
			uni_printf(io, "fp");
			break;
		case R_S0:
			uni_printf(io, "s0");
			break;
		case R_S1:
			uni_printf(io, "s1");
			break;

		case R_A0:
			uni_printf(io, "a0");
			break;
		case R_A1:
			uni_printf(io, "a1");
			break;
		case R_A2:
			uni_printf(io, "a2");
			break;
		case R_A3:
			uni_printf(io, "a3");
			break;
		case R_A4:
			uni_printf(io, "a4");
			break;
		case R_A5:
			uni_printf(io, "a5");
			break;
		case R_A6:
			uni_printf(io, "a6");
			break;
		case R_A7:
			uni_printf(io, "a7");
			break;

		case R_S2:
			uni_printf(io, "s2");
			break;
		case R_S3:
			uni_printf(io, "s3");
			break;
		case R_S4:
			uni_printf(io, "s4");
			break;
		case R_S5:
			uni_printf(io, "s5");
			break;
		case R_S6:
			uni_printf(io, "s6");
			break;
		case R_S7:
			uni_printf(io, "s7");
			break;
		case R_S8:
			uni_printf(io, "s8");
			break;
		case R_S9:
			uni_printf(io, "s9");
			break;
		case R_S10:
			uni_printf(io, "s10");
			break;
		case R_S11:
			uni_printf(io, "s11");
			break;

		case R_T3:
			uni_printf(io, "t3");
			break;
		case R_T4:
			uni_printf(io, "t4");
			break;
		case R_T5:
			uni_printf(io, "t5");
			break;
		case R_T6:
			uni_printf(io, "t6");
			break;

		case R_FT0:
			uni_printf(io, "ft0");
			break;
		case R_FT1:
			uni_printf(io, "ft1");
			break;
		case R_FT2:
			uni_printf(io, "ft2");
			break;
		case R_FT3:
			uni_printf(io, "ft3");
			break;
		case R_FT4:
			uni_printf(io, "ft4");
			break;
		case R_FT5:
			uni_printf(io, "ft5");
			break;
		case R_FT6:
			uni_printf(io, "ft6");
			break;
		case R_FT7:
			uni_printf(io, "ft7");
			break;

		case R_FS0:
			uni_printf(io, "fs0");
			break;
		case R_FS1:
			uni_printf(io, "fs1");
			break;

		case R_FA0:
			uni_printf(io, "fa0");
			break;
		case R_FA1:
			uni_printf(io, "fa1");
			break;
		case R_FA2:
			uni_printf(io, "fa2");
			break;
		case R_FA3:
			uni_printf(io, "fa3");
			break;
		case R_FA4:
			uni_printf(io, "fa4");
			break;
		case R_FA5:
			uni_printf(io, "fa5");
			break;
		case R_FA6:
			uni_printf(io, "fa6");
			break;
		case R_FA7:
			uni_printf(io, "fa7");
			break;

		case R_FS2:
			uni_printf(io, "fs2");
			break;
		case R_FS3:
			uni_printf(io, "fs3");
			break;
		case R_FS4:
			uni_printf(io, "fs4");
			break;
		case R_FS5:
			uni_printf(io, "fs5");
			break;
		case R_FS6:
			uni_printf(io, "fs6");
			break;
		case R_FS7:
			uni_printf(io, "fs7");
			break;
		case R_FS8:
			uni_printf(io, "fs8");
			break;
		case R_FS9:
			uni_printf(io, "fs9");
			break;
		case R_FS10:
			uni_printf(io, "fs10");
			break;
		case R_FS11:
			uni_printf(io, "fs11");
			break;
		case R_FS2:
			uni_printf(io, "fs0");
			break;

		case R_FT8:
			uni_printf(io, "ft8");
			break;
		case R_FT9:
			uni_printf(io, "ft9");
			break;
		case R_FT10:
			uni_printf(io, "ft10");
			break;
		case R_FT11:
			uni_printf(io, "ft11");
			break;
	}
}
