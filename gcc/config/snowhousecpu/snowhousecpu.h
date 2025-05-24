/* Target Definitions for snowhousecpu.
   Copyright (C) 2023 Free Software Foundation, Inc.
   Contributed by Andrew Clark (FL4SHK).

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_SNOWHOUSECPU_H
#define GCC_SNOWHOUSECPU_H

#include "../elfos.h"

#undef  STARTFILE_SPEC
//#define STARTFILE_SPEC "%{!mno-crt0:crt0%O%s} crti.o%s crtbegin.o%s"
#define STARTFILE_SPEC "%{!mno-crt0:crt0%O%s} crti.o%s crtbegin.o%s"

// Provide an ENDFILE_SPEC appropriate for svr4.  Here we tack on our own
// magical crtend.o file (see crtstuff.c) which provides part of the
// support for getting C++ file-scope static object constructed before
// entering `main', followed by the normal svr3/svr4 "finalizer" file,
// which is either `gcrtn.o' or `crtn.o'.

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s crtn.o%s"

// Provide a LIB_SPEC appropriate for svr4.  Here we tack on the default
// standard C library (unless we are building a shared library) and
// the simulator BSP code.

//#undef LIB_SPEC
//#define LIB_SPEC "%{!shared:%{!symbolic:-lc}}"
//#undef LIB_SPEC
//#define LIB_SPEC "%{!static:%{!symbolic:-lc}}"

//#undef LINK_SPEC
//#define LINK_SPEC "%{h*} %{v:-V} %{!mel:-EB} %{mel:-EL}
//    %{static:-Bstatic} %{shared:-shared} %{symbolic:-Bsymbolic}"
#undef LINK_SPEC
#define LINK_SPEC \
	"%{mel:-EL}" \
    "%{static:-static} " \
	"%{shared:%eshared is not supported} " \
	"%{--relax}"

//#ifndef MULTILIB_DEFAULTS
//#define MULTILIB_DEFAULTS { "meb" }
//#endif


#define BITS_PER_WORD 32

#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 0 //1
#define WORDS_BIG_ENDIAN 0 //1
//#define REG_WORDS_BIG_ENDIAN

#define UNITS_PER_WORD 4

// Layout of Source Language Data Types
#define CHAR_TYPE_SIZE 8
#define SHORT_TYPE_SIZE 16
#define INT_TYPE_SIZE 32
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64

//#define FLOAT_TYPE_SIZE 32
//#define DOUBLE_TYPE_SIZE 64
//#define LONG_DOUBLE_TYPE_SIZE 64

#define DEFAULT_SIGNED_CHAR 0

#undef SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE "int"

#undef WCHAR_TYPE
#define WCHAR_TYPE "unsigned int"

#undef  WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE BITS_PER_WORD

//#undef INTMAX_TYPE
//#define INTMAX_TYPE "long long"
//
//#undef UINTMAX_TYPE
//#define UINTMAX_TYPE "unsigned long long"

//#undef INT8_TYPE
//#define INT8_TYPE "signed char"
//#undef INT16_TYPE
//#define INT16_TYPE "short int"
//#undef INT32_TYPE
//#define INT32_TYPE "int"
//#undef INT64_TYPE
//#define INT64_TYPE "long long int"
//#undef UINT8_TYPE
//#define UINT8_TYPE "unsigned char"
//#undef UINT16_TYPE
//#define UINT16_TYPE "unsigned short int"
//#undef UINT32_TYPE
//#define UINT32_TYPE "unsigned int"
//#undef UINT64_TYPE
//#define UINT64_TYPE "unsigned long long int"
//
//#undef INT_LEAST8_TYPE
//#define INT_LEAST8_TYPE INT8_TYPE
//#undef INT_LEAST16_TYPE
//#define INT_LEAST16_TYPE INT16_TYPE
//#undef INT_LEAST32_TYPE
//#define INT_LEAST32_TYPE INT32_TYPE
//#undef INT_LEAST64_TYPE
//#define INT_LEAST64_TYPE INT64_TYPE
//#undef UINT_LEAST8_TYPE
//#define UINT_LEAST8_TYPE UINT8_TYPE
//#undef UINT_LEAST16_TYPE
//#define UINT_LEAST16_TYPE UINT16_TYPE
//#undef UINT_LEAST32_TYPE
//#define UINT_LEAST32_TYPE UINT32_TYPE
//#undef UINT_LEAST64_TYPE
//#define UINT_LEAST64_TYPE UINT64_TYPE
//
//#undef INT_FAST8_TYPE
//#define INT_FAST8_TYPE INT8_TYPE
//#undef INT_FAST16_TYPE
//#define INT_FAST16_TYPE INT16_TYPE
//#undef INT_FAST32_TYPE
//#define INT_FAST32_TYPE INT32_TYPE
//#undef INT_FAST64_TYPE
//#define INT_FAST64_TYPE INT64_TYPE
//#undef UINT_FAST8_TYPE
//#define UINT_FAST8_TYPE UINT8_TYPE
//#undef UINT_FAST16_TYPE
//#define UINT_FAST16_TYPE UINT16_TYPE
//#undef UINT_FAST32_TYPE
//#define UINT_FAST32_TYPE UINT32_TYPE
//#undef UINT_FAST64_TYPE
//#define UINT_FAST64_TYPE UINT64_TYPE
//
//#undef INTPTR_TYPE
//#define INTPTR_TYPE INT32_TYPE
//#undef UINTPTR_TYPE
//#define UINTPTR_TYPE UINT32_TYPE

// Registers
#define REGISTER_NAMES \
  { \
    /*"r0", */ \
    "r1", "r2", "r3", \
    "r4", "r5", "r6", "r7", \
    "r8", "r9", "r10", "r11", \
    "r12", "lr", "fp", "sp", \
    "fake_fp", "fake_ap", \
    /*"flags",*/ \
    /* "hi", "lo", */ \
    /*"ids", "ira", "ie", "ity", "sty",*/ \
    "pc" \
  }

//#define SNOWHOUSECPU_R0 0
#define SNOWHOUSECPU_R1 0
#define SNOWHOUSECPU_R2 1
#define SNOWHOUSECPU_R3 2
#define SNOWHOUSECPU_R4 3
#define SNOWHOUSECPU_R5 4
#define SNOWHOUSECPU_R6 5
#define SNOWHOUSECPU_R7 6
#define SNOWHOUSECPU_R8 7
#define SNOWHOUSECPU_R9 8
#define SNOWHOUSECPU_R10 9
#define SNOWHOUSECPU_R11 10
#define SNOWHOUSECPU_R12 11
#define SNOWHOUSECPU_LR 12
#define SNOWHOUSECPU_FP 13
#define SNOWHOUSECPU_SP 14
#define SNOWHOUSECPU_FAKE_FP 15
#define SNOWHOUSECPU_FAKE_AP 16
//#define SNOWHOUSECPU_FAKE_FLAGS 17
/*
#define SNOWHOUSECPU_HI 19
#define SNOWHOUSECPU_LO 20
*/
/*
#define SNOWHOUSECPU_IDS 21
#define SNOWHOUSECPU_IRA 22
#define SNOWHOUSECPU_IE 23
#define SNOWHOUSECPU_ITY 23
#define SNOWHOUSECPU_STY 24
#define SNOWHOUSECPU_PC 25
*/
//#define SNOWHOUSECPU_PC 18
#define SNOWHOUSECPU_PC 17

#define SNOWHOUSECPU_FIRST_ARG_REGNUM \
  (SNOWHOUSECPU_R1)
#define SNOWHOUSECPU_LAST_ARG_REGNUM \
  (SNOWHOUSECPU_R6) \
  /* (SNOWHOUSECPU_R3) */
#define SNOWHOUSECPU_NUM_ARG_REGS \
  (SNOWHOUSECPU_LAST_ARG_REGNUM - SNOWHOUSECPU_FIRST_ARG_REGNUM + 1)

#define SNOWHOUSECPU_FIRST_GENERAL_REGNUM \
  (SNOWHOUSECPU_R1)
#define SNOWHOUSECPU_LAST_GENERAL_REGNUM \
  (SNOWHOUSECPU_FAKE_AP)
#define SNOWHOUSECPU_NUM_GENERAL_REGS \
  (SNOWHOUSECPU_LAST_GENERAL_REGNUM - SNOWHOUSECPU_FIRST_GENERAL_REGNUM + 1)
#define STATIC_CHAIN_REGNUM \
  (SNOWHOUSECPU_LAST_ARG_REGNUM + 1)

/*
#define SNOWHOUSECPU_FIRST_INTERRUPT_REG
  SNOWHOUSECPU_IDS
#define SNOWHOUSECPU_LAST_INTERRUPT_REG
  SNOWHOUSECPU_STY
#define SNOWHOUSECPU_NUM_INTERRUPT_REGS
  (SNOWHOUSECPU_LAST_INTERRUPT_REG - SNOWHOUSECPU_FIRST_INTERRUPT_REG + 1)
*/

#define FIRST_PSEUDO_REGISTER \
  (SNOWHOUSECPU_PC + 1)

// If the program counter has a register number, define this as that
// register number.
#define PC_REGNUM SNOWHOUSECPU_PC

// Registers with fixed purposes marked with a "1". Registers here
#define FIXED_REGISTERS \
  { \
    /*0,*/ /* r0, */ \
    0, 0, 0,   /* r1, r2, r3 */ \
    0, 0, 0, 0,   /* r4, r5, r6, r7 */ \
    0, 0, 0, 0,   /* r8, r9, r10, r11 */ \
    0, 0, 1, 1,   /* r12, lr, fp, sp */ \
    1, 1,         /* fake_fp, fake_ap */ \
    /*1,*/            /* fake_flags */ \
    1,            /* pc */ \
  }

// Registers clobbered upon function call
// "r1"-"r6" used for arguments
// "r0" used for return value
// "lr" clobbered upon function call
//// "flags" clobbered upon function call
#define CALL_REALLY_USED_REGISTERS \
  { \
    /*1, */ /* r0 */ \
    1, 1, 1,      /* r1, r2, r3 */ \
    1, 1, 1, 0,   /* r4, r5, r6, r7 */ \
    0, 0, 0, 0,   /* r8, r9, r10, r11 */ \
    0, 1, 1, 1,   /* r12, lr, fp, sp */ \
    1, 1,         /* fake_fp, fake_ap */ \
    /*1,*/            /* fake_flags */ \
    1,            /* pc */ \
  }

//#define REG_ALLOC_ORDER
// {
//   0, 1, 2,       /* r1, r2, r3 */
//   3, 4, 5, 6,            /* r4, r5, r6, r7 */
//   7, 8, 9, 10,           /* r8, r9, r10, r11 */
//   11, 12, 13, 14,  /* r12, lr, fp, sp */
//   15, 16,        /* fake_fp, fake_ap */
//   17,                    /* pc */
// }

//#define HONOR_REG_ALLOC_ORDER 1

enum reg_class
{
  NO_REGS,
  GENERAL_REGS,
  //FULL_PRODUCT_RESULT_REGS,
  //FULL_PRODUCT_HIGH_PART_REGS,
  //FULL_PRODUCT_LOW_PART_REGS,
  FP_REGS,
  SP_REGS,
  //CC_REGS,
  PC_REGS,
  SPECIAL_REGS,
  ALL_REGS,
  LIM_REG_CLASSES,
};

#define N_REG_CLASSES LIM_REG_CLASSES

#define REG_CLASS_NAMES \
  { \
    "NO_REGS", \
    "GENERAL_REGS", \
    /* "FULL_PRODUCT_RESULT_REGS", */ \
    /* "FULL_PRODUCT_HIGH_PART_REGS", */ \
    /* "FULL_PRODUCT_LOW_PART_REGS", */ \
    "FP_REGS", \
    "SP_REGS", \
    /*"CC_REGS",*/ \
    "PC_REGS", \
    "SPECIAL_REGS", \
    "ALL_REGS", \
  }


// NO_REGS: Empty
// GENERAL_REGS: r1 to r12, lr, fp, sp, fake_fp, fake_ap
//// FULL_PRODUCT_RESULT_REGS: r0
//// CC_REGS: flags
// SPECIAL_REGS: pc
// HI_REGS: hi
// LO_REGS: lo
// HILO_REGS: hi, lo
// INTERRUPT_REGS: ids, ira, ie, ity, sty

#define REG_CLASS_CONTENTS \
  { \
    {0x0}, /* NO_REGS */ \
    {((1 << SNOWHOUSECPU_NUM_GENERAL_REGS) - 1) \
      << SNOWHOUSECPU_FIRST_GENERAL_REGNUM}, /* GENERAL_REGS */ \
    /*{0x3ffff},*/ /* GENERAL_REGS */ \
    /* {(1 << SNOWHOUSECPU_R0) | (1 << SNOWHOUSECPU_R1)}, */ \
      /* FULL_PRODUCT_RESULT_REGS */ \
    /* {1 << SNOWHOUSECPU_R0}, */ /* FULL_PRODUCT_HIGH_PART_REGS */ \
    /* {1 << SNOWHOUSECPU_R1}, */ /* FULL_PRODUCT_LOW_PART_REGS */ \
    {1 << SNOWHOUSECPU_FP}, /* FP_REGS */ \
    {1 << SNOWHOUSECPU_SP}, /* SP_REGS */ \
    /* {1 << SNOWHOUSECPU_FAKE_FLAGS}, */ /* CC_REGS */ \
    {1 << SNOWHOUSECPU_PC}, /* PC_REGS */ \
    {1 << SNOWHOUSECPU_PC}, /* SPECIAL_REGS */ \
    /* ALL_REGS:  All registers */ \
    {(1 << (FIRST_PSEUDO_REGISTER)) - 1}, \
  }


static const enum reg_class
snowhousecpu_regno_to_class[FIRST_PSEUDO_REGISTER] =
{
  // r0, 
  //FULL_PRODUCT_HIGH_PART_REGS, 
  //r1, r2, r3
  GENERAL_REGS,
    GENERAL_REGS, GENERAL_REGS,

  // r4, r5, r6, r7
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,

  // r8, r9, r10, r11
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,

  // r12, lr, fp, sp
  GENERAL_REGS, GENERAL_REGS, FP_REGS, SP_REGS,

  // fake_fp, fake_ap
  GENERAL_REGS, GENERAL_REGS,

  //// flags,
  //CC_REGS,

  //hi, lo, 
  //HI_REGS, LO_REGS,

  // ids, ira, ie, ity, sty
  //INTERRUPT_REGS, INTERRUPT_REGS, INTERRUPT_REGS,
  //INTERRUPT_REGS, INTERRUPT_REGS, 

  //pc
  //SPECIAL_REGS,
  PC_REGS,
};


// A C expression whose value is a register class containing hard register
// REGNO.
#define REGNO_REG_CLASS(R) \
  ((R < FIRST_PSEUDO_REGISTER) ? snowhousecpu_regno_to_class[R] : NO_REGS)

//#define IRA_HARD_REGNO_ADD_COST_MULTIPLIER(jegno)
//  ((regno) <= SNOWHOUSECPU_LAST_ARG_REGNUM ? 0.0 : 0.65)


// Force GCC to accept that SNOWHOUSECPU_LR is to be restored during the
// epilogue
#define EPILOGUE_USES(r) (r == SNOWHOUSECPU_LR)

// EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
// the stack pointer does not matter.  The value is tested only in
// functions that have frame pointers.
// No definition is equivalent to always zero.
//#define EXIT_IGNORE_STACK 1

//// A C expression that is nonzero if it is permissible to store a
//// value of mode MODE in hard register number REGNO (or in several
//// registers starting with that one).
//#define TARGET_HARD_REGNO_MODE_OK(REGNO, MODE) 1


/*
// Define this macro if it is advisable to hold scalars in registers
// in a wider mode than that declared by the program.  In such cases,
// the value is constrained to be within the bounds of the declared
// type, but kept valid in the wider mode.  The signedness of the
// extension may differ from that of the type.

#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE) \
  do \
  { \
    if ((GET_MODE_CLASS (MODE) == MODE_INT) \
      && (GET_MODE_SIZE (MODE) < 4)) \
    { \
      (MODE) = SImode; \
    } \
  } while (0)
*/

// Define this macro if the compiler should avoid copies to/from CCmode
// registers. You should only define this macro if support for copying
// to/from CCmode is incomplete.
//// (Change this later as it *is* possible to back up/restore the flags)
#define AVOID_CCMODE_COPIES 1





// The Overall Framework of an Assembler File
#undef ASM_SPEC
#define ASM_COMMENT_START "//"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""

//#define ASM_FPRINTF_EXTENSIONS(FILE, ARGS, P )
//  case '/'


// Switch to the text or data segment.
//#define TEXT_SECTION_ASM_OP  "\t.text"
//#define DATA_SECTION_ASM_OP  "\t.data"

/* Output before read-only data.  */
#define TEXT_SECTION_ASM_OP "\t.section\t.text"

/* Output before writable data.  */
#define DATA_SECTION_ASM_OP "\t.section\t.data"

/* Output before uninitialized data.  */
#define BSS_SECTION_ASM_OP "\t.section\t.bss"

#define CTORS_SECTION_ASM_OP "\t.section\t.init_array,\"aw\",%init_array"
#define DTORS_SECTION_ASM_OP "\t.section\t.fini_array,\"aw\",%fini_array"

#define HAS_INIT_SECTION

#undef INIT_SECTION_ASM_OP
#undef FINI_SECTION_ASM_OP
#define INIT_ARRAY_SECTION_ASM_OP CTORS_SECTION_ASM_OP
#define FINI_ARRAY_SECTION_ASM_OP DTORS_SECTION_ASM_OP

/* Since we use .init_array/.fini_array we don't need the markers at
   the start and end of the ctors/dtors arrays.  */
#define CTOR_LIST_BEGIN asm (CTORS_SECTION_ASM_OP)
#define CTOR_LIST_END           /* empty */
#define DTOR_LIST_BEGIN asm (DTORS_SECTION_ASM_OP)
#define DTOR_LIST_END           /* empty */

// Assembler Commands for Alignment

/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE,LOG)      \
  if ((LOG) != 0)                       \
    fprintf (FILE, "\t.align %d\n", (1 << (LOG)))

//#undef ASM_OUTPUT_SKIP
//#define ASM_OUTPUT_SKIP(FILE,SIZE)
//  fprintf (FILE, "\t.skip " HOST_WIDE_INT_PRINT_UNSIGNED"\n", (SIZE))

/* This says how to output an assembler line
   to define a global common symbol.  */
//#undef ASM_OUTPUT_COMMON
//#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)  
//( fputs ("\t.common ", (FILE)),         
//  assemble_name ((FILE), (NAME)),               
//  fprintf ((FILE), "," HOST_WIDE_INT_PRINT_UNSIGNED",\"bss\"\n", (SIZE)))

/* Return a nonzero value if DECL has a section attribute.  */
#define IN_NAMED_SECTION_P(DECL)					\
  ((TREE_CODE (DECL) == FUNCTION_DECL || VAR_P (DECL))	\
   && DECL_SECTION_NAME (DECL) != NULL)

/* This says how to output an assembler line to define a local common
   symbol.  */

#undef  ASM_OUTPUT_ALIGNED_DECL_LOCAL
#define ASM_OUTPUT_ALIGNED_DECL_LOCAL(FILE, DECL, NAME, SIZE, ALIGN)	\
  do									\
    {									\
      if ((DECL) != NULL && IN_NAMED_SECTION_P (DECL))			\
	switch_to_section (get_named_section (DECL, NULL, 0));		\
      else								\
	switch_to_section (bss_section);				\
									\
      ASM_OUTPUT_ALIGN (FILE, floor_log2 (ALIGN / BITS_PER_UNIT));	\
      ASM_OUTPUT_LABEL (FILE, NAME);					\
      fprintf (FILE, "\t.space\t%d\n", SIZE ? (int) SIZE : 1);		\
      fprintf (FILE, "\t.size\t%s, %d\n",				\
	       NAME, SIZE ? (int) SIZE : 1);				\
    }									\
  while (0)

/* #define ASM_OUTPUT_ALIGNED_LOCAL(FILE, NAME, SIZE, ALIGNED)          */ \
/* ( fputs ("\t.space ", (FILE)),                                       */ \
/*   assemble_name ((FILE), (NAME)),                            */      \
/*   fprintf ((FILE), "," HOST_WIDE_INT_PRINT_UNSIGNED",\"bss\",%u\n",  */ \
/*         (SIZE), ((ALIGNED) / BITS_PER_UNIT)))                */

/* A C statement (sans semicolon) to output to the stdio stream
   FILE the assembler definition of uninitialized global DECL named
   NAME whose size is SIZE bytes and alignment is ALIGN bytes.
   Try to use asm_output_aligned_bss to implement this macro.  */
#undef  ASM_OUTPUT_ALIGNED_BSS
#define ASM_OUTPUT_ALIGNED_BSS(FILE, DECL, NAME, SIZE, ALIGN)   	\
  do									\
    {									\
      if (IN_NAMED_SECTION_P (DECL))					\
	switch_to_section (get_named_section (DECL, NULL, 0));		\
      else								\
	switch_to_section (bss_section);				\
      									\
      ASM_OUTPUT_ALIGN (FILE, floor_log2 (ALIGN / BITS_PER_UNIT));	\
									\
      last_assemble_variable_decl = DECL;				\
      ASM_DECLARE_OBJECT_NAME (FILE, NAME, DECL);			\
      ASM_OUTPUT_SKIP (FILE, SIZE ? (int)(SIZE) : 1);			\
    } 									\
  while (0)




/* Output #ident as a .ident.  */

#undef TARGET_ASM_OUTPUT_IDENT
#define TARGET_ASM_OUTPUT_IDENT default_asm_output_ident_directive



//#define ASM_OUTPUT_ALIGN(STREAM,POWER)
//  fprintf (STREAM, "//\n", /*"\t.p2align\t%d\n"*/, POWER);

// Output and Generation of Labels

#define GLOBAL_ASM_OP ".global "


#define INIT_SECTION_ASM_OP ".section .init"


// Passing Function Arguments

// A C expression. If nonzero, push insns will be used to pass outgoing
// arguments. If the target machine does not have a push instruction, set
// it to zero.
#define PUSH_ARGS 0
//#define PUSH_ROUNDING (npushed)
//  npushed

// A C type for declaring a variable that is used as the first
// argument of `FUNCTION_ARG' and other related values.
#define CUMULATIVE_ARGS unsigned int


// A C expression. If nonzero, the maximum amount of space required for
// outgoing arguments will be computed and placed into
// crtl->outgoing_args_size. No space will be pushed onto the stack for
// each call; instead, the function prologue should increase the stack
// frame size by this amount.  Setting both PUSH_ARGS and
// ACCUMULATE_OUTGOING_ARGS is not proper.
#define ACCUMULATE_OUTGOING_ARGS 1

//// Define this if it is the responsibility of the caller to allocate
//// the area reserved for arguments passed in registers.
//#define REG_PARM_STACK_SPACE(FNDECL) (6 * UNITS_PER_WORD)

// A C statement (sans semicolon) for initializing the variable CUM
// for the state at the beginning of the argument list.
// For snowhousecpu, the first arg is passed in register 0 (aka r0).
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS) \
  /* (CUM = SNOWHOUSECPU_R0) */ \
  (CUM = SNOWHOUSECPU_FIRST_ARG_REGNUM)


// How Scalar Function Values Are Returned

// STACK AND CALLING

// Stack frame setup:
// High
//                    ------------------------
//                    last on-stack parameter
//                    on-stack parameters (args grow upwards)...
// old sp, fake_ap, dwarf cfa, on-stack parameter 0x0
//                    ------------------------
// fake_fp            old fp (if `frame_pointer_needed`)
//                    ------------------------
// fp,                callee-saved register save 0x0
//                    callee-saved registers save area...
//                    last callee-saved register save
//                    ------------------------
//                    local variable save area
//                    ------------------------
//                    padding for alignment
//                    ------------------------
//                    last outgoing arg
//                    outgoing args (args grow upward)...
//                    outgoing arg 0x0
//                    (and sometimes dynamically allocated object on the 
//                    stack)
// sp
// Low

//// Because Flare32 is big-endian, 0xaabbccdd is laid out in memory like
//// this:
//// [addr+0x0:0xaa, addr+0x1:0xbb, addr+0x2:0xcc, addr+0x3:0xdd]
//// 


// Define this macro if pushing a word onto the stack moves the stack
// pointer to a smaller address.
#define STACK_GROWS_DOWNWARD 1

// Define this macro to nonzero value if the addresses of local variable
// slots are at negative offsets from the frame pointer.
#define FRAME_GROWS_DOWNWARD 1

// Offset from the stack pointer register to the first location at which 
// outgoing arguments are placed.
// If not specified, the default value of zero is used.
// This is the proper value for most machines.
// If `ARGS_GROW_DOWNWARD, this is the offset to the location above the
// first location at which outgoing arguments are placed.
// We have `!ARGS_GROW_DOWNWARD`
#define STACK_POINTER_OFFSET 0
//#define STACK_POINTER_OFFSET (UNITS_PER_WORD)
//#define STACK_POINTER_OFFSET (snowhousecpu_stack_pointer_offset ())

// Offset from the stack pointer register to an item dynamically allocated
// on the stack, e.g., by alloca.
// The default value for this macro is STACK_POINTER_OFFSET plus the length 
// of the outgoing arguments. The default is correct for most machines.
// See function.cc for details.
//#define STACK_DYNAMIC_OFFSET(FNDECL)
  

// Offset from the argument pointer register to the first argument’s
// address.  If ARGS_GROW_DOWNWARD, this is the offset to the location
// above the first argument’s address.
//// We have `ARGS_GROW_DOWNWARD`, so we're at the address *above* 
//// the first argument's address, per the "Stack frame setup" comments
//#define FIRST_PARM_OFFSET(fundecl) UNITS_PER_WORD
#define FIRST_PARM_OFFSET(fundecl) 0x0

//#define FIRST_PARM_OFFSET(funcdel) (snowhousecpu_first_parm_offset (funcdecl))

#undef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE NO_DEBUG

// Don't add this until adding in DWARF2 stuff
//#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (Pmode, SNOWHOUSECPU_LR)
////#define DWARF_FRAME_RETURN_COLUMN SNOWHOUSECPU_LR
//#define DWARF_FRAME_RETURN_COLUMN DWARF_FRAME_REGNUM (SNOWHOUSECPU_LR)

#define RETURN_ADDR_RTX(C, F) snowhousecpu_get_return_address (C)

// This macro need only be defined if the target might save registers in
// the function prologue at an offset to the stack pointer that is not
// aligned to UNITS_PER_WORD. The definition should be the negative minimum 
// alignment if STACK_GROWS_DOWNWARD is true, and the positive minimum
// alignment otherwise.
// See Section 18.21.2 [DWARF], page 653. Only applicable if the target
// supports DWARF 2 frame unwind information.
//#define DWARF_CIE_DATA_ALIGNMENT (-UNITS_PER_WORD)

// The CFA includes the pretend args.
// FL4SHK Note: I thought the CFA actually didn't include the
// `pretend_args`, but it seems that it does on a lot of machines, so I'm
// including it here
#define ARG_POINTER_CFA_OFFSET(FNDECL) \
  (gcc_assert ((FNDECL) == current_function_decl), \
   FIRST_PARM_OFFSET (FNDECL) + crtl->args.pretend_args_size)
// We have `!ARGS_GROW_DOWNWARD`

//// Per the stack frame's structure, the argument pointer is at either one
//// or two words away from the frame pointer
//#define ARG_POINTER_CFA_OFFSET(FNDECL)
//  (gcc_assert ((FNDECL) == current_function_decl),
//  FIRST_PARM_OFFSET (FNDECL)
//    + (-UNITS_PER_WORD)
//    + (frame_pointer_needed ? (-UNITS_PER_WORD) : 0x0))

// Per the stack frame's structure, `fake_ap` is at the same address as 
//#define ARG_POINTER_CFA_OFFSET(FNDECL)
//  (gcc_assert ((FNDECL) == current_function_decl),
//  0x0)


//// If defined, a C expression whose value is an integer giving the offset
//// in bytes from the frame pointer to the canonical frame address (cfa).
//// The final value should coincide with that calculated by
//// INCOMING_FRAME_SP_OFFSET.  Normally the CFA is calculated as an offset
//// from the argument pointer, via ARG_POINTER_CFA_OFFSET, but if the
//// argument pointer is variable due to the ABI, this may not be possible.
//// If this macro is defined, it implies that the virtual register
//// instantiation should be based on the frame pointer instead of the
//// argument pointer.  Only one of FRAME_POINTER_CFA_OFFSET and
//// ARG_POINTER_CFA_OFFSET should be defined.
//#define FRAME_POINTER_CFA_OFFSET(funcdecl) UNITS_PER_WORD

// A C expression whose value is an integer giving the offset, in bytes,
// from the value of the stack pointer register to the top of the stack
// frame at the beginning of any function, before the prologue.
// The top of the frame is defined to be the value of the stack pointer in
// the previous frame, just before the call instruction.
// You only need to define this macro if you want to support call frame
// debugging information like that provided by DWARF 2.
//#define INCOMING_FRAME_SP_OFFSET (snowhousecpu_incoming_frame_sp_offset ())
#define INCOMING_FRAME_SP_OFFSET 0x0

//// Define this macro if successive arguments to a function occupy decreasing
//// addresses on the stack.

// Describe how we implement __builtin_eh_return.
#define EH_RETURN_DATA_REGNO(N) ((N) < 4 ? N : INVALID_REGNUM)

// Store the return handler into the call frame.
#define EH_RETURN_HANDLER_RTX \
  gen_frame_mem (Pmode, \
    plus_constant (Pmode, frame_pointer_rtx, UNITS_PER_WORD))

//#define ARGS_GROW_DOWNWARD 1


// This macro defines the operation used when something is pushed on the
// stack.  In RTL, a push operation will be
// (set (mem (STACK_PUSH_CODE (reg sp))) ....)
#define STACK_PUSH_CODE POST_DEC
#define STACK_POP_CODE PRE_INC



// Registers That Address the Stack Frame


// The register number of the stack pointer register, which must also
// be a fixed register according to `FIXED_REGISTERS'.
#define STACK_POINTER_REGNUM SNOWHOUSECPU_SP

// The register number of the frame pointer register, which is used to
// access automatic variables in the stack frame.
#define FRAME_POINTER_REGNUM SNOWHOUSECPU_FAKE_FP
//#define FRAME_POINTER_REGNUM SNOWHOUSECPU_FP

// The register number of the arg pointer register, which is used to
// access the function's argument list.
#define ARG_POINTER_REGNUM SNOWHOUSECPU_FAKE_AP
//#define ARG_POINTER_REGNUM SNOWHOUSECPU_FP

#define HARD_FRAME_POINTER_REGNUM SNOWHOUSECPU_FP


// Addressing modes

// Specify the machine mode that pointers have.
// After generation of rtl, the compiler makes no further distinction
// between pointers and any other objects of this machine mode.
#define Pmode SImode

// A macro whose definition is the name of the class to which a
// valid base register must belong.  A base register is one used in
// an address which is the register value plus a displacement.
#define BASE_REG_CLASS GENERAL_REGS

// A macro whose definition is the name of the class to which a
// valid index register must belong.  An index register is one used
// in an address where its value is either multiplied by a scale
// factor or added to another register (as well as added to a
// displacement).
//#define INDEX_REG_CLASS BASE_REG_CLASS
#define INDEX_REG_CLASS NO_REGS

// An alias for the machine mode used for memory references to
// functions being called, in `call' RTL expressions.
//// This is HImode, because instruction addresses are 16-bit aligned.
// This is SImode, because instruction addresses are 32-bit aligned.
//#define FUNCTION_MODE HImode
#define FUNCTION_MODE SImode

#define ELIMINABLE_REGS \
  { \
    {ARG_POINTER_REGNUM, STACK_POINTER_REGNUM}, \
    {ARG_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM}, \
    {FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM}, \
    {FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM} \
  }

/*
#define ELIMINABLE_REGS \
  {{ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM}}
*/

// This macro returns the initial difference between the specified pair
// of registers.
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET) \
  do \
  { \
    (OFFSET) = snowhousecpu_initial_elimination_offset ((FROM), (TO)); \
  } while (0)

// A C expression that is nonzero if REGNO is the number of a hard
// register in which function arguments are sometimes passed.
#define FUNCTION_ARG_REGNO_P(r) \
  (r >= SNOWHOUSECPU_FIRST_ARG_REGNUM && r <= SNOWHOUSECPU_LAST_ARG_REGNUM)


#define HARD_REGNO_OK_FOR_BASE_P(NUM) \
  (((unsigned) (NUM) < FIRST_PSEUDO_REGISTER) \
    && (REGNO_REG_CLASS (NUM) == GENERAL_REGS \
      || (NUM) == HARD_FRAME_POINTER_REGNUM \
      || (NUM) == STACK_POINTER_REGNUM))
#define HARD_REGNO_OK_FOR_INDEX_P(NUM) \
  false
  /*(((unsigned) (NUM) < FIRST_PSEUDO_REGISTER)*/ \
    /*&& (REGNO_REG_CLASS (NUM) == GENERAL_REGS*/ \
    /*  || (NUM) == HARD_FRAME_POINTER_REGNUM*/ \
    /*  || (NUM) == STACK_POINTER_REGNUM))*/





#ifdef REG_OK_STRICT
// A C expression which is nonzero if register number NUM is suitable
// for use as a base register in operand addresses.
#define REGNO_OK_FOR_BASE_P(NUM) \
  (HARD_REGNO_OK_FOR_BASE_P (NUM) \
  || HARD_REGNO_OK_FOR_BASE_P (reg_renumber[(NUM)]))

// A C expression which is nonzero if register number NUM is suitable
// for use as an index register in operand addresses.
#define REGNO_OK_FOR_INDEX_P(NUM) \
  (HARD_REGNO_OK_FOR_INDEX_P (NUM) \
  || HARD_REGNO_OK_FOR_INDEX_P (reg_renumber[(NUM)]))
#else
// A C expression which is nonzero if register number NUM is suitable
// for use as a base register in operand addresses.
#define REGNO_OK_FOR_BASE_P(NUM)   \
  ((NUM) >= FIRST_PSEUDO_REGISTER || HARD_REGNO_OK_FOR_BASE_P (NUM))

// A C expression which is nonzero if register number NUM is suitable
// for use as an index register in operand addresses.
#define REGNO_OK_FOR_INDEX_P(NUM) \
  false /*((NUM) >= FIRST_PSEUDO_REGISTER || HARD_REGNO_OK_FOR_INDEX_P (NUM))*/
#endif
//#define REGNO_OK_FOR_INDEX_P(NUM) SNOWHOUSECPU_FP

//#define REGNO_OK_FOR_INDEX_P(NUM) 0

// The maximum number of bytes that a single instruction can move
// quickly between memory and registers or between two memory
// locations.
#define MOVE_MAX UNITS_PER_WORD

// Not all loads zero extend, but other backends that have both zero extend
// and sign extend loads do this.
#define LOAD_EXTEND_OP(MEM) ZERO_EXTEND

// Short immediates definitely do sign extend.
#define SHORT_IMMEDIATES_SIGN_EXTEND 1

// A number, the maximum number of registers that can appear in a
// valid memory address.
// Registers are 32-bit, and addresses are also 32-bit.
//#define MAX_REGS_PER_ADDRESS 3
//#define MAX_REGS_PER_ADDRESS 2
#define MAX_REGS_PER_ADDRESS 1

// An alias for a machine mode name.  This is the machine mode that
// elements of a jump-table should have.
//#define CASE_VECTOR_MODE HImode
#define CASE_VECTOR_MODE SImode


// Describing Relative Costs of Operations
// Define this macro as a C expression which is nonzero if accessing
// less than a word of memory (i.e. a `char' or a `short') is no
// faster than accessing a word of memory.
#define SLOW_BYTE_ACCESS 1

// It's generally better to use an immediate-encoded function address for
// calls than to keep the function address in a register.
// Most function calls can be done with "bl".
#define NO_FUNCTION_CSE true



// Stuff for trampolines
#define TRAMPOLINE_SIZE 1
//#define TRAMPOLINE_ALIGNMENT BITS_PER_WORD
#define TRAMPOLINE_ALIGNMENT 32
#define TRAMPOLINE_DATA_ALIGNMENT 32

#define TARGET_CPU_CPP_BUILTINS() \
  { \
    builtin_define_std ("snowhousecpu"); \
    builtin_define_std ("SNOWHOUSECPU"); \
    builtin_define ("__SNOWHOUSECPU__"); \
  }

//#undef TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
//#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P snowhousecpu_legitimate_address_p



// Normal alignment required for function parameters on the stack, in bits.
// All stack parameters receive at least this much alignment regardless of
// data type. On most machines, this is the same as the size of an integer.
#define PARM_BOUNDARY 32

// Define this macro to the minimum alignment enforced by hardware for the
// stack pointer on this machine. The definition is a C expression for the
// desired alignment (measured in bits). This value is used as a default if
// PREFERRED_STACK_BOUNDARY is not defined. On most machines, this should
// be the same as PARM_BOUNDARY.
#define STACK_BOUNDARY 32

// Alignment required for a function entry point, in bits.
#define FUNCTION_BOUNDARY 32

// Biggest alignment that any data type can require on this machine, in
// bits.  Note that this is not the biggest alignment that is supported,
// just the biggest alignment that, when violated, may cause a fault.
#define BIGGEST_ALIGNMENT 32

#define FASTEST_ALIGNMENT 32


// Set this nonzero if move instructions will actually fail to work
// when given unaligned data.
#define STRICT_ALIGNMENT 1


// Alignment of field after `int : 0' in a structure.
#define EMPTY_FIELD_BOUNDARY 32

// Look at the fundamental type that is used for a bit-field and use 
// that to impose alignment on the enclosing structure.
// struct s {int a:8}; should have same alignment as "int", not "char".
#define PCC_BITFIELD_TYPE_MATTERS 1

// Largest integer machine mode for structures.  If undefined, the default
// is GET_MODE_SIZE (DImode).
#define MAX_FIXED_MODE_SIZE 32

// borrowed from `../arm/arm.h`
#define SNOWHOUSECPU_EXPAND_ALIGNMENT(COND, EXP, ALIGN) \
  ( \
    ( \
      (COND) && ((ALIGN) < BITS_PER_WORD) \
      && ( \
        TREE_CODE (EXP) == ARRAY_TYPE \
        || TREE_CODE (EXP) == UNION_TYPE \
        || TREE_CODE (EXP) == RECORD_TYPE \
      ) \
    ) ? FASTEST_ALIGNMENT : (ALIGN) \
  )

// Align global data.
#define DATA_ALIGNMENT(EXP, ALIGN)   \
  (SNOWHOUSECPU_EXPAND_ALIGNMENT (!optimize_size, EXP, ALIGN))

// Always align objects on the stack
#define LOCAL_ALIGNMENT(EXP, ALIGN) \
  (SNOWHOUSECPU_EXPAND_ALIGNMENT (true, EXP, ALIGN))

#define STRUCTURE_SIZE_BOUNDARY FASTEST_ALIGNMENT

#undef USER_LABEL_PREFIX
#define USER_LABEL_PREFIX "_"

/* This is how to store into the string LABEL
   the symbol_ref name of an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.
   This is suitable for output with `assemble_name'.  */

#undef ASM_GENERATE_INTERNAL_LABEL
#define ASM_GENERATE_INTERNAL_LABEL(LABEL,PREFIX,NUM)   \
  sprintf ((LABEL), "*.%s%ld", (PREFIX), (long)(NUM))

// Generating Code for Profiling
#define FUNCTION_PROFILER(FILE,LABELNO) (abort (), 0)
//#define FUNCTION_PROFILER(FILE,LABELNO) do { } while (0)

#define HAS_LONG_COND_BRANCH true
#define HAS_LONG_UNCOND_BRANCH true

#define TARGET_OBJECT_SUFFIX ".o"

extern int reload_in_progress;

#endif    // GCC_SNOWHOUSECPU_H
