/* Target Code for snowhousecpu
   Copyright (C) 2023 Free Software Foundation, Inc.

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
#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "df.h"
#include "regs.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "expr.h"
#include "builtins.h"
#include "explow.h"
#include "print-rtl.h"

#include "snowhousecpu-protos.h"

__attribute__ ((used)) void
snowhousecpu_dump_file_debug_rtx (FILE* file, const_rtx x)
{
  rtx_writer w (file, 0, false, false, NULL);
  w.print_rtx (x);
  fprintf (file, "\n");
}
/* This file should be included last.  */
#include "target-def.h"

//#define SNOWHOUSECPU_DEBUG
//#define SNOWHOUSECPU_CONSTRAINT_DEBUG
//#define SNOWHOUSECPU_CONSTRAINT_4ADDR_DEBUG
//#define SNOWHOUSECPU_STACK_DEBUG

#ifdef SNOWHOUSECPU_STACK_DEBUG
#define snowhousecpu_stack_debug_fprintf(...) \
    do { fprintf (__VA_ARGS__); } while (0)
#else
#define snowhousecpu_stack_debug_fprintf(...) \
    do { } while (0)
#endif

#ifdef SNOWHOUSECPU_DEBUG
#define snowhousecpu_debug_fprintf(...) \
  do { fprintf (__VA_ARGS__); } while (0)
#define snowhousecpu_debug_rtx(...) \
  do { debug_rtx (__VA_ARGS__); } while (0)
#else
#define snowhousecpu_debug_fprintf(...) do { } while (0)
#define snowhousecpu_debug_rtx(...) do { } while (0)
#endif

#ifdef SNOWHOUSECPU_CONSTRAINT_DEBUG
#define snowhousecpu_constraint_debug_fprintf(...) \
  do { fprintf (__VA_ARGS__); } while (0)
#define snowhousecpu_constraint_debug_rtx(...) \
  do { debug_rtx (__VA_ARGS__); } while (0)
#else
#define snowhousecpu_constraint_debug_fprintf(...) do { } while (0)
#define snowhousecpu_constraint_debug_rtx(...) do { } while (0)
#endif

#ifdef SNOWHOUSECPU_CONSTRAINT_4ADDR_DEBUG
#define snowhousecpu_constraint_4addr_debug_fprintf(...) \
  do { fprintf (__VA_ARGS__); } while (0)
#define snowhousecpu_constraint_4addr_debug_rtx(...) \
  do { debug_rtx (__VA_ARGS__); } while (0)
#else
#define snowhousecpu_constraint_4addr_debug_fprintf(...) do { } while (0)
#define snowhousecpu_constraint_4addr_debug_rtx(...) do { } while (0)
#endif

// Per-function machine data.
struct GTY (()) machine_function
{
  /* Number of bytes saved on the stack for callee saved registers. */
  int callee_saved_reg_size;

  /* Number of bytes saved on the stack for local variables.
    Includes padding for alignment once.  */
  int local_vars_size;

  /* Number of bytes of arguments passed onto the stack. */
  int stack_args_size;

  HOST_WIDE_INT outgoing_args_size;

  /* The sum of 2 sizes: locals vars and padding byte for saving the
    registers.  Used in expand_prologue () and expand_epilogue (). */
  int size_for_adjusting_sp;

  /* Padding for alignment */
  //int
  //  //padding_stack_args,
  //  //padding_locals;

  /* # of bytes of static stack space allocated by the function. */
  int static_stack_size;

  //int varargs_extra_stack_size;
};

static constexpr int STACK_ALIGNMENT = STACK_BOUNDARY / BITS_PER_UNIT;

#define SNOWHOUSECPU_STACK_ALIGN(LOC) \
  (ROUND_UP ((LOC), STACK_ALIGNMENT))

static void
update_stack_args_size (struct machine_function* self)
{
  // Padding needed for each element of the frame.
  self->stack_args_size = crtl->args.pretend_args_size;
  snowhousecpu_stack_debug_fprintf (
    stderr,
    "snowhousecpu debug: update_stack_args_size(): %u\n",
    self->stack_args_size
  );
}

static void
update_local_vars_size (struct machine_function* self)
{
  // Padding needed for each element of the frame.
  self->local_vars_size = (
    SNOWHOUSECPU_STACK_ALIGN
    (HOST_WIDE_INT (get_frame_size ()))
    //+ (HOST_WIDE_INT (self->varargs_extra_stack_size))
  );
  snowhousecpu_stack_debug_fprintf (
    stderr,
    "snowhousecpu debug: update_local_vars_size(): %u\n",
    self->local_vars_size
  );
}

static void
update_callee_saved_reg_size (struct machine_function* self)
{
  self->callee_saved_reg_size = 0;

  // Save callee-saved registers
  for (int regno=0; regno<FIRST_PSEUDO_REGISTER; ++regno)
  {
    if (snowhousecpu_regno_actually_callee_saved (regno))
    //if (snowhousecpu_regno_actually_callee_saved_no_hfp (regno))
    //if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
    {
      self->callee_saved_reg_size += UNITS_PER_WORD;
      snowhousecpu_stack_debug_fprintf (
        stderr,
        "snowhousecpu debug: update_callee_saved_reg_size(): INNER: reg_names[%u]=%s\n",
        regno,
        reg_names[regno]
      );
    }
  }
  //snowhousecpu_debug_fprintf (stderr, "update_callee_saved_reg_size (): %i\n",
  //  self->callee_saved_reg_size);
  snowhousecpu_stack_debug_fprintf (
    stderr,
    "snowhousecpu debug: update_callee_saved_reg_size(): END: %u\n",
    self->callee_saved_reg_size
  );
}

static void
update_outgoing_args_size (struct machine_function* self)
{
  self->outgoing_args_size
    = ACCUMULATE_OUTGOING_ARGS
      ? HOST_WIDE_INT (crtl->outgoing_args_size)
      : 0;
  //snowhousecpu_debug_fprintf (stderr, "update_outgoing_args_size (): %i\n",
  //  self->outgoing_args_size);
  snowhousecpu_stack_debug_fprintf (
    stderr,
    "snowhousecpu debug: update_outgoing_args_size(): %u\n",
    self->outgoing_args_size
  );
}

static void
update_size_for_adjusting_sp (struct machine_function* self)
{
  //machine.size_for_adjusting_sp = crtl->args.pretend_args_size
  //  + machine.local_vars_size
  //  + (ACCUMULATE_OUTGOING_ARGS
  //  ? static_cast<HOST_WIDE_INT>(crtl->outgoing_args_size) : 0);

  //size_for_adjusting_sp = stack_args_size + UNITS_PER_WORD
  //  + self->callee_saved_reg_size
  //  + self->local_vars_size
  //  + (ACCUMULATE_OUTGOING_ARGS
  //  ? HOST_WIDE_INT(crtl->outgoing_args_size) : 0);
  //self->size_for_adjusting_sp
  //  = self->local_vars_size
  //  + (ACCUMULATE_OUTGOING_ARGS
  //    ? HOST_WIDE_INT(crtl->outgoing_args_size)
  //    : 0);
  //self->size_for_adjusting_sp
  //  = self->local_vars_size
  //  + self->outgoing_args_size;
  self->size_for_adjusting_sp
    = self->stack_args_size //crtl->args.pretend_args_size
    + self->local_vars_size
    + self->outgoing_args_size;
  //snowhousecpu_debug_fprintf (stderr, "update_size_for_adjusting_sp (): %i\n",
  //  self->size_for_adjusting_sp);
  snowhousecpu_stack_debug_fprintf (
    stderr,
    "snowhousecpu debug: update_size_for_adjusting_sp(): %u\n",
    self->size_for_adjusting_sp
  );
}
static void
update_static_stack_size (struct machine_function* self)
{
  self->static_stack_size
    = (frame_pointer_needed ? UNITS_PER_WORD : 0)
    + self->stack_args_size
    + self->callee_saved_reg_size
    + self->local_vars_size
    + self->outgoing_args_size;
  //snowhousecpu_debug_fprintf (stderr, "update_static_stack_size (): %i\n",
  //  self->static_stack_size);
  snowhousecpu_stack_debug_fprintf (
    stderr,
    "snowhousecpu debug: update_static_stack_size(): %u\n",
    self->static_stack_size
  );
}

static void
snowhousecpu_compute_frame (struct machine_function* self)
{
  update_stack_args_size (self);
  update_local_vars_size (self);
  update_callee_saved_reg_size (self);
  update_outgoing_args_size (self);
  update_size_for_adjusting_sp (self);
  update_static_stack_size (self);
}
static void
snowhousecpu_compute_frame ()
{
  snowhousecpu_stack_debug_fprintf (
    stderr,
    "snowhousecpu debug: current_function_name(): %s\n",
    current_function_name ()
  );
  //update_stack_args_size (cfun->machine);
  //update_local_vars_size (cfun->machine);
  //update_callee_saved_reg_size (cfun->machine);
  //update_outgoing_args_size (cfun->machine);
  //update_size_for_adjusting_sp (cfun->machine);
  //update_static_stack_size (cfun->machine);
  snowhousecpu_compute_frame (cfun->machine);
  snowhousecpu_stack_debug_fprintf (
    stderr,
    "\n"
  );
}

#define LOSE_AND_RETURN(msgid, x) \
do \
{ \
  snowhousecpu_operand_lossage (msgid, x); \
  return; \
} while (0)

//static void
//print_poly_int (FILE *file, poly_int64 x)
//{
//  HOST_WIDE_INT const_x;
//  if (x.is_constant (&const_x))
//    fprintf (file, HOST_WIDE_INT_PRINT_DEC, const_x);
//  else
//    {
//      fprintf (file, "[" HOST_WIDE_INT_PRINT_DEC, x.coeffs[0]);
//      for (int i = 1; i < NUM_POLY_INT_COEFFS; ++i)
//        fprintf (file, ", " HOST_WIDE_INT_PRINT_DEC, x.coeffs[i]);
//      fprintf (file, "]");
//    }
//}
//static void
//snowhousecpu_dbg_print_rtx (rtx x)
//{
//  if (GET_CODE (x) == REG)
//  {
//    fprintf (stderr, "%s:%s:%d",
//      GET_RTX_NAME (GET_CODE (x)),
//      //GET_MODE (x) == SImode,
//      //GET_MODE_SIZE (GET_MODE (x))
//      GET_MODE_NAME (GET_MODE (x)),
//      REGNO (x));
//  }
//  else if (GET_CODE (x) == SUBREG)
//  {
//    //fprintf (stderr, "SUBREG:{%d, %d, %d}",
//    //  REGNO (SUBREG_REG (x)),
//    //  GET_MODE (x) == SImode,
//    //  GET_MODE_SIZE (GET_MODE (x)));
//    //fprintf (stderr, "%s:%s",
//    //  GET_RTX_NAME (GET_CODE (x)),
//    //  GET_MODE_NAME (GET_MODE (x)));
//    //fprintf (stderr,
//    //  //"%s:%s:%s:%d:%d:%d:%d:",
//    //  "%s:%s:%s:%d:%d:%d:%",
//    //  //"%s:%s:%d:%d",
//    //  GET_RTX_NAME (GET_CODE (x)),
//    //  GET_MODE_NAME (GET_MODE (x)),
//    //  GET_MODE_NAME (GET_MODE (SUBREG_REG (x))),
//    //  REGMODE_NATURAL_SIZE (GET_MODE (x)),
//    //  REGNO (SUBREG_REG (x)),
//    //  REGMODE_NATURAL_SIZE (GET_MODE (SUBREG_REG (x)))
//    //  //paradoxical_subreg_p (x)
//    //  //SUBREG_PROMOTED_UNSIGNED_P (x)
//    //  //SUBREG_BYTE (x)
//    //  );
//    //print_poly_int (stderr, SUBREG_BYTE (x));
//    fprintf (stderr,
//      "%s:%s[",
//      GET_RTX_NAME (GET_CODE (x)),
//      GET_MODE_NAME (GET_MODE (x)));
//    snowhousecpu_dbg_print_rtx (SUBREG_REG (x));
//    //fprintf (stderr, ":%d",
//    //  SUBREG_BYTE (x));
//    fprintf (stderr, "]:");
//    print_poly_int (stderr, SUBREG_BYTE (x));
//  }
//  else if (snowhousecpu_absolute_ex (x))
//  {
//    fprintf (stderr, "%s:%s",
//      GET_RTX_NAME (GET_CODE (x)),
//      GET_MODE_NAME (GET_MODE (x)));
//  }
//  else if (GET_CODE (x) == MEM)
//  {
//    fprintf (stderr, "%s:%s[",
//      GET_RTX_NAME (GET_CODE (x)),
//      //GET_MODE (x) == SImode,
//      //GET_MODE_SIZE (GET_MODE (x))
//      GET_MODE_NAME (GET_MODE (x)));
//    snowhousecpu_dbg_print_rtx (XEXP (x, 0));
//    fprintf (stderr, "]");
//  }
//  else if (GET_CODE (x) == PLUS)
//  {
//    rtx
//      left = XEXP (x, 0),
//      right = XEXP (x, 1);
//    fprintf (stderr, "(%s:%s ",
//      GET_RTX_NAME (GET_CODE (x)),
//      //GET_MODE (x) == SImode,
//      //GET_MODE_SIZE (GET_MODE (x))
//      GET_MODE_NAME (GET_MODE (x)));
//    snowhousecpu_dbg_print_rtx (left);
//    fprintf (stderr, " ");
//    snowhousecpu_dbg_print_rtx (right);
//    fprintf (stderr, ")");
//  }
//  else
//  {
//    fprintf (stderr, "%s:%s",
//      //unsigned(GET_CODE (x)),
//      GET_RTX_NAME (GET_CODE (x)),
//      //GET_MODE (x) == SImode,
//      //GET_MODE_SIZE (GET_MODE (x))
//      GET_MODE_NAME (GET_MODE (x)));
//  }
//}

//static constexpr int MIN_SIMM18 = -131072;
//static constexpr int MAX_SIMM18 = 131071;

// Define how to find the value returned by a function.
// VALTYPE is the data type of the value (as a tree).
// If the precise function being called is known, FUNC is its
// FUNCTION_DECL; otherwise, FUNC is 0.
//

// We always return values in register r1 for snowhousecpu.
// This may need to be adjusted this for DImode?
static rtx
snowhousecpu_function_value (const_tree valtype,
  const_tree fntype_or_decl ATTRIBUTE_UNUSED,
  bool outgoing ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (TYPE_MODE (valtype), SNOWHOUSECPU_R1);
}

// Define how to find the value returned by a library function.
//
// We always return values in register r1 for snowhousecpu.
// This may need to be adjusted this for DImode?
static rtx
snowhousecpu_libcall_value (machine_mode mode,
  const_rtx fun ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (mode, SNOWHOUSECPU_R1);
}

// Handle TARGET_FUNCTION_VALUE_REGNO_P.
//
// We always return values in register r1 for snowhousecpu.
// This may need to be adjusted this for DImode?
static bool
snowhousecpu_function_value_regno_p (const unsigned int regno)
{
  return (regno == SNOWHOUSECPU_R1);
}

//static rtx
//snowhousecpu_static_chain (const_tree ARG_UNUSED (fndecl_or_type),
//  bool incoming_p)
//{
//  rtx addr, mem;
//
//  if (incoming_p)
//  {
//    addr = plus_constant (Pmode, arg_pointer_rtx, 2 * UNITS_PER_WORD);
//  }
//  else
//  {
//    addr = plus_constant (Pmode, stack_pointer_rtx, -UNITS_PER_WORD);
//  }
//
//  mem = gen_rtx_MEM (Pmode, addr);
//  MEM_NOTRAP_P (mem) = 1;
//
//  return mem;
//}

/* Worker function for TARGET_ASM_TRAMPOLINE_TEMPLATE.  */

static void
snowhousecpu_asm_trampoline_template (FILE *f)
{
  ////fprintf (f, "\tpush  $sp, $r0\n");
  ////fprintf (f, "\tldi.l $r0, 0x0\n");
  ////fprintf (f, "\tsto.l 0x8($fp), $r0\n");
  ////fprintf (f, "\tpop   $sp, $r0\n");
  ////fprintf (f, "\tjmpa  0x0\n");


  ////// Allocate space on the stack for the `pop pc` instruction.
  ////// This is could be faster in the general case than just pushing a random
  ////// register onto the stack to allocate space on the stack because there
  ////// is no memory access.
  ////// Admittedly, this may only really be relevant for implementations of
  ////// SnowHouseCpu that do not have any data cache.
  //////asm_fprintf (f, "\tadd %r, #-4\n", STACK_POINTER_REGNUM);
  ////asm_fprintf (f, "\tpush r1\n");

  ////// 0x2 == 0x0 + 0x2
  ////asm_fprintf (f, "\tadd r1, pc, 0xe - 0x2\n");

  ////// 0x4 == 0x2 + 0x2
  ////asm_fprintf (f, "\tldr %r, [r1]\n", STATIC_CHAIN_REGNUM);

  ////// 16-bit instruction encoding, due to relaxing in GAS, so we don't need
  ////// to use `str.nr`.
  ////// 0x6 = 0x4 + 0x2
  ////asm_fprintf (f, "\tldr r1, [r1, #0x4]\n");

  ////// 16-bit instruction encoding, due to relaxing in GAS, so we don't need
  ////// to use `str.nr`.
  ////// 0x8 = 0x6 + 0x2
  ////asm_fprintf (f, "\tstr r1, [%r, #8]\n", STACK_POINTER_REGNUM);

  ////// 0xa = 0x8 + 0x2
  ////asm_fprintf (f, "\tpop r1\n");

  ////// 0xc = 0xa + 0x2
  ////asm_fprintf (f, "\tpop pc\n");

  ////// 0xe = 0x
  ////assemble_aligned_integer (TRAMPOLINE_DATA_ALIGNMENT, const0_rtx);
  ////assemble_aligned_integer (TRAMPOLINE_DATA_ALIGNMENT, const0_rtx);
  ////assemble_aligned_integer (TRAMPOLINE_DATA_ALIGNMENT, const0_rtx);
  ////assemble_aligned_integer (TRAMPOLINE_DATA_ALIGNMENT, const0_rtx);
  ////asm_fprintf (f, "\tpush(r1),\n");
  ////asm_fprintf (f, "\tpop(pc),\n");
  //asm_fprintf (f, "\tsub(%r, %r, 4),\n", STACK_POINTER_REGNUM, STACK_POINTER_REGNUM);
  //asm_fprintf (f, "\tstr(r1, %r, 0),\n", STACK_POINTER_REGNUM);
}
//
///* Worker function for TARGET_TRAMPOLINE_INIT.  */
//
//static void
//snowhousecpu_trampoline_init (rtx m_tramp, tree fndecl, rtx chain_value)
//{
//  rtx mem, fnaddr = XEXP (DECL_RTL (fndecl), 0);
//
//  emit_block_move (m_tramp, assemble_trampoline_template (),
//                  GEN_INT (TRAMPOLINE_SIZE), BLOCK_OP_NORMAL);
//
//  mem = adjust_address (m_tramp, SImode, 4);
//  emit_move_insn (mem, chain_value);
//  mem = adjust_address (m_tramp, SImode, 16);
//  emit_move_insn (mem, fnaddr);
//}

// Emit an error message when we're in an asm, and a fatal error for
// "normal" insns.  Formatted output isn't easily implemented, since we
// use output_operand_lossage to output the actual message and handle the
// categorization of the error.
static void
snowhousecpu_operand_lossage (const char *msgid, rtx op)
{
  debug_rtx (op);
  output_operand_lossage ("%s", msgid);
}


static bool
snowhousecpu_valid_base_reg_p (rtx reg, bool strict_p)
{
  //const int regno = REGNO (reg);
  bool ret = false;
  snowhousecpu_debug_fprintf (stderr,
    "snowhousecpu_valid_base_reg_p () start: %d\n",
    int(strict_p));
  snowhousecpu_debug_rtx (reg);

  if (!strict_p && GET_CODE (reg) == SUBREG)
  {
    //reg = SUBREG_REG (reg);
    reg
      = simplify_gen_subreg (SImode, reg, DImode,
        subreg_lowpart_offset (SImode, DImode));
    snowhousecpu_debug_fprintf (stderr, "!strict_p && SUBREG:\n");
    snowhousecpu_debug_rtx (reg);
  }

  //if (GET_MODE (reg) == Pmode)
  if (REG_P (reg)
    //|| GET_CODE (reg) == SUBREG
  )
  {
    if (strict_p)
    {
      snowhousecpu_debug_fprintf (stderr, "REG_P with strict_p: %d:%d:%d:%d\n",
        REGNO (reg),
        reg_renumber[REGNO (reg)],
        HARD_REGNO_OK_FOR_BASE_P (REGNO (reg)),
        HARD_REGNO_OK_FOR_BASE_P (reg_renumber[REGNO (reg)]));
      ret = (HARD_REGNO_OK_FOR_BASE_P (REGNO (reg))
        || HARD_REGNO_OK_FOR_BASE_P (reg_renumber[REGNO (reg)]));
    }
    else
    {
      //if (GET_CODE (reg) == SUBREG)
      //{
      //  reg = SUBREG_REG (reg);
      //}
      snowhousecpu_debug_fprintf (stderr, "REG_P with !strict_p: %d:%d:%d\n",
        REGNO (reg),
        !HARD_REGISTER_NUM_P (REGNO (reg)),
        (
          //!HARD_REGISTER_NUM_P (REGNO (reg))
          //? 0
          //:
          HARD_REGNO_OK_FOR_BASE_P (REGNO (reg))
        )
      );

      ret = (!HARD_REGISTER_NUM_P (REGNO (reg))
        || HARD_REGNO_OK_FOR_BASE_P (REGNO (reg)));
    }
  }
  snowhousecpu_debug_fprintf (stderr, "snowhousecpu_valid_base_reg_p () end: %d\n",
    int(ret));
  snowhousecpu_debug_fprintf (stderr, "\n");

  return ret;
}

//extern rtx_insn *curr_insn;
static bool
snowhousecpu_legitimate_address_p (machine_mode mode, rtx x, bool strict_p,
  addr_space_t as, code_helper my_code_helper ATTRIBUTE_UNUSED)
{
  snowhousecpu_debug_fprintf (stderr,
    "snowhousecpu_legitimate_address_p (): begin: %d\n",
    unsigned(GET_MODE_SIZE (mode)));

  gcc_assert (ADDR_SPACE_GENERIC_P (as));
  snowhousecpu_debug_fprintf (stderr,
    "snowhousecpu_legitimate_address_p (): post gcc_assert: %d\n",
    unsigned(GET_MODE_SIZE (mode)));

  // --------
  // DImode
  {
    //if (GET_MODE_SIZE (mode) > UNITS_PER_WORD)
    //{
    //  snowhousecpu_debug_fprintf (stderr,
    //    "snowhousecpu_legitimate_address_p (): dbg 0: %s\n",
    //    GET_MODE_NAME (mode));
    //  //snowhousecpu_debug_rtx (curr_insn);
    //  snowhousecpu_debug_rtx (x);
    //  snowhousecpu_debug_fprintf (stderr, "\n");
    //}
    if (GET_CODE (x) == REG)
    {
      if (snowhousecpu_valid_base_reg_p (x, strict_p))
      {
        return true;
      }
    }
    if (GET_CODE (x) == PLUS
      //&& GET_CODE (XEXP (x, 0)) == REG
      //&& snowhousecpu_valid_base_reg_p (XEXP (x, 0), strict_p)
    )
    {
      rtx
        left_0 = XEXP (x, 0),
        right_0 = XEXP (x, 1);
      snowhousecpu_debug_fprintf (stderr,
        "snowhousecpu_legitimate_address_p (): plus:\n");
      snowhousecpu_debug_rtx (left_0);
      snowhousecpu_debug_rtx (right_0);
      if (snowhousecpu_valid_base_reg_p (left_0, strict_p))
      {
        if (snowhousecpu_absolute_ex (right_0))
        {
          return true;
        }
      }
    }
    //else if (GET_CODE == REG)
    //{
    //}
    //if (snowhousecpu_valid_base_reg_p (left_0, strict_p)
    //  && GET_CODE (right_0) == PLUS)
    //{
    //  rtx
    //    left_1 = XEXP (right_0, 0),
    //    right_1 = XEXP (right_0, 1);
    //}
  }
  // --------
  if (snowhousecpu_absolute_ex (x))
  //if (snowhousecpu_absolute (x))
  {
    snowhousecpu_debug_fprintf (stderr,
      "snowhousecpu_legitimate_address_p (): absolute: %s\n",
      GET_MODE_NAME (mode));
    //snowhousecpu_debug_rtx (curr_insn);
    snowhousecpu_debug_rtx (x);
    return true;
  }
  // --------
  snowhousecpu_debug_fprintf (stderr,
    "snowhousecpu_legitimate_address_p (): dbg: ");
  snowhousecpu_debug_rtx (x);
  return false;
}

// The PRINT_OPERAND worker.
static void
snowhousecpu_print_operand (FILE *file, rtx x, int code)
{
  rtx operand = x;

  int rgoff = 0;
  int the_regno = 0;
  switch (code)
  {
    case 0:
      // No code, print as usual
      break;

    case 'H':
      // Higher of a register pair, print one up
      rgoff = 1;
      break;

    case 'R':
    case 'L':
      // Lower of a register pair, print normal
      rgoff = 0;
      break;
    case 'C':
    {
      const enum rtx_code cond_code = GET_CODE (operand);
      switch (cond_code)
      {
        case EQ:
          fprintf (file, "eq");
          return;
        case NE:
          fprintf (file, "ne");
          return;
        case GEU:
          fprintf (file, "geu");
          return;
        case LTU:
          fprintf (file, "ltu");
          return;
        case GTU:
          fprintf (file, "gtu");
          return;
        case LEU:
          fprintf (file, "leu");
          return;
        case GE:
          fprintf (file, "ges");
          return;
        case LT:
          fprintf (file, "lts");
          return;
        case GT:
          fprintf (file, "gts");
          return;
        case LE:
          fprintf (file, "les");
          return;
        default:
          LOSE_AND_RETURN ("invalid condition code", x);
      }
    }

    default:
      //fprintf (stderr, "snowhousecpu_print_operand (): dbg 0\n");
      LOSE_AND_RETURN ("invalid operand modifier letter", x);
      break;
  }

  /* Print an operand as without a modifier letter.  */
  switch (GET_CODE (operand))
  {
    case REG:
      the_regno = REGNO (operand) + rgoff;
      if (the_regno >= FIRST_PSEUDO_REGISTER)
      {
        internal_error ("internal error: bad register: %d",
          the_regno);
      }
      fprintf (file, "%s", reg_names[the_regno]);
      break;

    case MEM:
      output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
      break;

    case CONST_DOUBLE:
      //if (CONST_DOUBLE_P (operand))
      //{
      //memcpy
      //}
      //fprintf (file, "%s");
    {
      //fprintf (file, "0x%lx%08lx", l[1] & 0xffffffff, l[0] & 0xffffffff);
      if (GET_MODE_SIZE (GET_MODE (operand)) == UNITS_PER_WORD)
      {
        long l;
        REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (x), l);
        fprintf (file, "0x%04lx",
          l & 0xffffffff);
      }
      else if (GET_MODE_SIZE (GET_MODE (operand)) > UNITS_PER_WORD)
      {
        long l[2];
        REAL_VALUE_TO_TARGET_DOUBLE (*CONST_DOUBLE_REAL_VALUE (x), l);
        fprintf (file, "0x%04lx, #0x%04lx",
          l[0] & 0xffffffff, l[1] & 0xffffffff);
      }
      else
      {
        LOSE_AND_RETURN ("unexpected operand", x);
      }
    }
      break;

    default:
      // No need to handle all strange variants, let
      // output_addr_const do it for us.
      if (CONSTANT_P (operand))
      {
        //fprintf (file, "#");
        output_addr_const (file, operand);
        return;
      }

      //fprintf (stderr, "snowhousecpu_print_operand (): dbg 1\n");

      LOSE_AND_RETURN ("unexpected operand", x);
  }

  // New code entries should just be added to the switch below.  If
  // handling is finished, just return.  If handling was just a
  // modification of the operand, the modified operand should be put in
  // "operand", and then do a break to let default handling
  // (zero-modifier) output the operand.
}

//static void
//snowhousecpu_unique_section (tree decl, int reloc)
//{
//  bool one_only = DECL_ONE_ONLY (decl) && !HAVE_COMDAT_GROUP;
//  const char *suffix = NULL;
//  const char *linkonce;
//
//  switch (categorize_decl_for_section (decl, reloc))
//  {
//              case SECCAT_TEXT:
//                      suffix = one_only ? ".t" : ".text";
//                      break;
//              default:
//                      default_unique_section (decl, reloc);
//                      return;
//                      break;
//  }
//  //id = DECL_ASSEMBLER_NAME (decl);
//  //ultimate_transparent_alias_target (&id);
//  //name = IDENTIFIER_POINTER (id);
//  //name = targetm.strip_name_encoding (name);
//
//  /* If we're using one_only, then there needs to be a .gnu.linkonce
//     prefix to the section name.  */
//  linkonce = one_only ? ".gnu.linkonce" : "";
//
//  char *string = ACONCAT ((linkonce, suffix, NULL));
//
//  set_decl_section_name (decl, string);
//      return;
//}
//static void
//snowhousecpu_print_addr_const (FILE* file, rtx x)
//{
//  switch (GET_CODE (x))
//  {
//    case LABEL_REF:
//      //output_addr_const (file, x);
//      //break;
//    case SYMBOL_REF:
//      fprintf (file, "\"");
//      output_addr_const (file, x);
//      fprintf (file, "\"");
//      break;
//    case CONST:
//    case CONST_INT:
//      //fprintf (file, "<some_symbol_ref>");
//
//      //fprintf (file, "\"");
//      output_addr_const (file, x);
//      //fprintf (file, "\"");
//      break;
//  }
//}

static void
snowhousecpu_print_operand_address (FILE* file,
  machine_mode mode ATTRIBUTE_UNUSED, rtx x)
{
  auto snowhousecpu_print_addr_const = [](FILE* file, rtx x) -> void {
    switch (GET_CODE (x))
    {
      case LABEL_REF:
        //output_addr_const (file, x);
        //break;
      case SYMBOL_REF:
        //fprintf (file, "\"");
        output_addr_const (file, x);
        //fprintf (file, "\"");
        break;
      case CONST:
      //case CONST_INT:
        //fprintf (file, "<some_symbol_ref>");

        //fprintf (file, "\"");
        output_addr_const (file, x);
        //fprintf (file, "\"");
        break;
      default:
        gcc_unreachable ();
    }
  };
  rtx left, right;
  switch (GET_CODE (x))
  {
  case REG:
    fprintf (file, "%s, 0x0", reg_names[REGNO (x)]);
    break;

  case PLUS:
    left = XEXP (x, 0);
    right = XEXP (x, 1);
    if (GET_CODE (left) == REG)
    {
      fprintf (file, "%s, ", reg_names[REGNO (left)]);
      if (GET_CODE (right) == CONST_INT)
      {
        fprintf (file, "%ld", INTVAL (right));
      }
      else if (
        // CONSTANT_P (right)
        snowhousecpu_absolute (right)
      )
      {
        //fprintf (file, "\"");
        //output_addr_const (file, right);
        //fprintf (file, "\"");
        snowhousecpu_print_addr_const (file, right);
      }
      else
      {
        gcc_unreachable ();
      }
    }
    else
    {
      gcc_unreachable ();
    }

    break;

  //case LABEL_REF:
  //  fprintf (file, "<some_label_ref>");
  //  break;

  case LABEL_REF:
  //  output_addr_const (file, x);
  //  break;
  case SYMBOL_REF:
  //  fprintf (file, "\"");
  //  output_addr_const (file, x);
  //  fprintf (file, "\"");
  //  break;
  case CONST:
    snowhousecpu_print_addr_const (file, x);
    break;
  case CONST_INT:
  //  //fprintf (file, "<some_symbol_ref>");

  //  //fprintf (file, "\"");
  //  output_addr_const (file, x);
  //  //fprintf (file, "\"");
    fprintf (file, "%ld", INTVAL (x));
    break;

  //case CONST:
  //  fprintf (file, "<some_const>");
  //  break;

  default:
    gcc_unreachable ();
    break;
  }
}

extern bool
register_operand (rtx op, machine_mode mode);


bool
snowhousecpu_offset_address_reg_plus_imm_p (rtx x)
{
  bool ret = false;
  snowhousecpu_constraint_debug_fprintf (stderr,
    "(offset base + imm) start:\n");
  snowhousecpu_constraint_debug_rtx (x);
  //snowhousecpu_constraint_debug_fprintf (stderr, "\n");

  //if (!MEM_P (x))
  //{
  //  return false;
  //}
  if (MEM_P (x))
  {
    // Inside the MEM
    x = XEXP (x, 0);
    if (GET_CODE (x) == SUBREG)
    {
      //left_0 = SUBREG_REG (left_0);
      x
        = simplify_gen_subreg (SImode, x, DImode,
          subreg_lowpart_offset (SImode, DImode));
      snowhousecpu_constraint_debug_fprintf (stderr, "x SUBREG:\n");
      snowhousecpu_constraint_debug_rtx (x);
    }
    if (
      GET_CODE (x) == REG
      && REGNO_OK_FOR_BASE_P (REGNO (x))
    )
    {
      ret = true;
    }
    else if (GET_CODE (x) == PLUS)
    {
      rtx
        left_0 = XEXP (x, 0),
        right_0 = XEXP (x, 1);

      //#ifndef REG_OK_STRICT
      if (GET_CODE (left_0) == SUBREG)
      {
        //left_0 = SUBREG_REG (left_0);
        left_0
          = simplify_gen_subreg (SImode, left_0, DImode,
            subreg_lowpart_offset (SImode, DImode));
        snowhousecpu_constraint_debug_fprintf (stderr, "left_0 SUBREG:\n");
        snowhousecpu_constraint_debug_rtx (left_0);
      }
      //#endif // REG_OK_STRICT
      snowhousecpu_constraint_debug_fprintf (stderr,
        //"before if (...) ret = true: %d:%d:%d\n",
        "before if (...) ret = true: %d:%d\n",
        int(GET_CODE (left_0) == REG),
        //int(REGNO_OK_FOR_BASE_P (REGNO (left_0))),
        int(snowhousecpu_absolute_ex (right_0)));

      if (
        GET_CODE (left_0) == REG
        && REGNO_OK_FOR_BASE_P (REGNO (left_0))
        && snowhousecpu_absolute_ex (right_0)
      )
      {
        ret = true;
      }
    }
  }
  snowhousecpu_constraint_debug_fprintf (stderr,
    "(offset base + imm) end: %d\n",
    int(ret));
  snowhousecpu_constraint_debug_fprintf (stderr, "\n");
  return ret;
}


//void
//snowhousecpu_expand_compare_and_swap (rtx operands[])
//{
//}

// Compute the size of the local area and the size to be adjusted by the
// prologue and epilogue.
// Implements the macro INITIAL_ELIMINATION_OFFSET, return the OFFSET.

int
snowhousecpu_initial_elimination_offset (int from, int to)
{
  int ret = 0;
  //--------
  // FL4SHK Note: seems that, at least for SnowHouseCpu, we want the offset as
  // (from) - (to).
  if (from == ARG_POINTER_REGNUM
    && to == STACK_POINTER_REGNUM)
  {
    snowhousecpu_compute_frame ();

    //ret = UNITS_PER_WORD;
    //ret
    //  = -UNITS_PER_WORD
    //  //- cfun->machine->stack_args_size
    //    // we have `!ARGS_GROW_DOWNWARD`, so we don't need to include
    //    // stack_args_size in this calculation
    //  - (frame_pointer_needed ? UNITS_PER_WORD : 0)
    //  - cfun->machine->callee_saved_reg_size
    //  - cfun->machine->local_vars_size
    //  - cfun->machine->outgoing_args_size;
    ret
      = //+UNITS_PER_WORD
      //+ cfun->machine->stack_args_size
        // we have `!ARGS_GROW_DOWNWARD`, so we don't need to include
        // stack_args_size in this calculation
      + (frame_pointer_needed ? UNITS_PER_WORD : 0)
      + cfun->machine->callee_saved_reg_size
      + cfun->machine->local_vars_size
      //- cfun->machine->varargs_extra_stack_size
      + cfun->machine->outgoing_args_size;
  }
  else if (from == ARG_POINTER_REGNUM
    && to == HARD_FRAME_POINTER_REGNUM)
  {
    // if frame_pointer_needed, then we have saved `fp`
    //ret = -UNITS_PER_WORD
    //  - (frame_pointer_needed ? UNITS_PER_WORD : 0);
    ret = //+UNITS_PER_WORD
      + (frame_pointer_needed ? UNITS_PER_WORD : 0);
  }
  else if (from == FRAME_POINTER_REGNUM
    && to == STACK_POINTER_REGNUM)
  {
    snowhousecpu_compute_frame ();

    // if `frame_pointer_needed`, there's technically a "push fp", but the 
    // compiler perhaps doesn't need to really know about the "push fp".
    // The hard frame pointer is ABOVE the callee-saved registers
    // preservation area (actually points at the very first
    // location in the callee-saved registers preservation area).

    ret
      = //-cfun->machine->callee_saved_reg_size
        + (frame_pointer_needed ? UNITS_PER_WORD : 0x0)
      + cfun->machine->callee_saved_reg_size
      + cfun->machine->local_vars_size
      //- cfun->machine->varargs_extra_stack_size
      + cfun->machine->outgoing_args_size;
    //ret
    //  = -UNITS_PER_WORD
    //  - cfun->machine->local_vars_size
    //  - cfun->machine->outgoing_args_size;
  }
  else if (from == FRAME_POINTER_REGNUM
    && to == HARD_FRAME_POINTER_REGNUM)
  {
    snowhousecpu_compute_frame ();

    // if `frame_pointer_needed`, there's technically a "push fp", but the 
    // compiler perhaps doesn't need to really know about the "push fp".
    // The hard frame pointer is ABOVE the callee-saved registers
    // preservation area (actually points at the very first
    // location in the callee-saved registers preservation area).
    ret = frame_pointer_needed ? UNITS_PER_WORD : 0x0;
  }
  else
  {
    gcc_unreachable ();
  }

  return ret;
}

static bool
snowhousecpu_return_in_memory
  (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  const HOST_WIDE_INT size = int_size_in_bytes (type);
  return (size == -1 || size > 2 * UNITS_PER_WORD);
  //return ((size == -1) || (size > UNITS_PER_WORD));
}


// Return non-zero if the function argument described by TYPE is to be
// passed by reference.
static bool 
snowhousecpu_pass_by_reference (cumulative_args_t ca ATTRIBUTE_UNUSED,
  //machine_mode mode, const_tree type, bool named ATTRIBUTE_UNUSED
  const function_arg_info& arg
  )
{
  if (arg.aggregate_type_p ())
  {
    return true;
  }
  unsigned HOST_WIDE_INT size = arg.type_size_in_bytes ();
  return size > UNITS_PER_WORD * SNOWHOUSECPU_NUM_ARG_REGS;
}

/* Some function arguments will only partially fit in the registers
   that hold arguments.  Given a new arg, return the number of bytes
   that fit in argument passing registers.  */
static int
snowhousecpu_arg_partial_bytes (cumulative_args_t ca_v,
  //machine_mode mode, tree type, bool named
  const function_arg_info& arg
)
{
  if (!arg.named) {
    return 0;
  }

  CUMULATIVE_ARGS *ca = get_cumulative_args (ca_v);
  int bytes_left, size;

  if (*ca >= SNOWHOUSECPU_NUM_ARG_REGS)
  {
    return 0;
  }

  if (snowhousecpu_pass_by_reference (ca_v, arg))
  {
    size = UNITS_PER_WORD;
  }
  else if (arg.type)
  {
    if (AGGREGATE_TYPE_P (arg.type))
    {
      return 0;
    }
    size = int_size_in_bytes (arg.type);
  }
  else
  {
    size = GET_MODE_SIZE (arg.mode);
  }

  bytes_left = (UNITS_PER_WORD * SNOWHOUSECPU_NUM_ARG_REGS);

  if (size > bytes_left)
  {
    return bytes_left;
  }
  else
  {
    return 0;
  }
}//

// Return the next register to be used to hold a function argument or
// NULL_RTX if there's no more space.
static rtx
snowhousecpu_function_arg (cumulative_args_t ca_v,
  //machine_mode mode, const_tree type ATTRIBUTE_UNUSED,
  //bool named ATTRIBUTE_UNUSED
  const function_arg_info& arg
  )
{
  machine_mode mode = arg.mode;

  if (!arg.named)
  {
    return NULL_RTX;
  }

  CUMULATIVE_ARGS* ca = get_cumulative_args (ca_v);

  if (
    //(*ca) <= SNOWHOUSECPU_LAST_ARG_REGNUM - 1
    //(*ca) < SNOWHOUSECPU_NUM_ARG_REGS
    (*ca) < SNOWHOUSECPU_NUM_ARG_REGS
  )
  {
    // Check for trying to store larger-than-word-size arguments in
    // `cat(r5, r6)`
    if (GET_MODE_SIZE (mode) > UNITS_PER_WORD
      //&& (*ca) == SNOWHOUSECPU_LAST_ARG_REGNUM - 1)
      && (*ca) == SNOWHOUSECPU_NUM_ARG_REGS - 1)
    {
      return NULL_RTX;
    }

    const auto temp = (*ca) + SNOWHOUSECPU_FIRST_ARG_REGNUM;
    //fprintf (
    //  stderr, "\nsnowhousecpu_function_arg REG:  %u %s\n",
    //  *ca,
    //  reg_names[temp]
    //);
    return gen_rtx_REG (mode, temp);
  }
  else
  {
    //fprintf (stderr, "\nsnowhousecpu_function_arg NULL_RTX\n");
    return NULL_RTX;
  }
}

#define SNOWHOUSECPU_FUNCTION_ARG_SIZE(MODE, TYPE) \
  ((MODE) != BLKmode ? GET_MODE_SIZE (MODE) \
  : unsigned(int_size_in_bytes (TYPE)))

// This hook need not do anything if the argument in question was passed on
// the stack.
static void
snowhousecpu_function_arg_advance (cumulative_args_t ca_v,
  //machine_mode mode,
  //const_tree type, bool named ATTRIBUTE_UNUSED
  const function_arg_info& arg
  )
{
  if (!arg.named)
  {
    return;
  }

  CUMULATIVE_ARGS* ca = get_cumulative_args (ca_v);

  *ca = (
    //((*ca) <= SNOWHOUSECPU_LAST_ARG_REGNUM - 1
    ((*ca) < SNOWHOUSECPU_NUM_ARG_REGS)
    ? *ca + ((3 + SNOWHOUSECPU_FUNCTION_ARG_SIZE (arg.mode, arg.type))
      / UNITS_PER_WORD)
    : *ca);
}


bool
snowhousecpu_absolute (rtx x)
{
  return (
    GET_CODE (x) == SYMBOL_REF
    || GET_CODE (x) == LABEL_REF
    || GET_CODE (x) == CONST
    //|| GET_CODE (x) == CONST_INT
  );
}
bool
snowhousecpu_absolute_ex (rtx x)
{
  return (
    snowhousecpu_absolute (x)
    || (
      GET_CODE (x) == CONST_INT
      //&& (HOST_WIDE_INT (INTVAL (x))
      //  & ((HOST_WIDE_INT (1) << HOST_WIDE_INT (32)) - HOST_WIDE_INT (1)))
      //  == HOST_WIDE_INT (INTVAL (x))
    )
    //|| GET_CODE (x) == CONST_DOUBLE
  );
}

// Zero initialization is OK for all current fields.
static machine_function*
snowhousecpu_init_machine_status ()
{
  return ggc_cleared_alloc<machine_function>();
}



static void
snowhousecpu_option_override ()
{
  // Set the per-function-data initializer.
  init_machine_status = snowhousecpu_init_machine_status;
}

bool
snowhousecpu_regno_actually_callee_saved (int regno)
{
  // Don't need to preserve "pc"
  // Need to preserve "lr" if it gets used.
  // "df_regs_ever_live_p" checks whether or not the register is
  // used in this function.

  if (regno == HARD_FRAME_POINTER_REGNUM
    || regno == SNOWHOUSECPU_PC)
  {
    return false;
  }

  if (df_regs_ever_live_p (regno))
  {
    //fprintf (
    //  stderr,
    //  "debugging: df_regs_ever_live_p (%i)\n",
    //  regno
    //);
    if (regno == SNOWHOUSECPU_HI)
    {
      return false;
    }
    if (regno == SNOWHOUSECPU_FAKE_FP || regno == SNOWHOUSECPU_FAKE_AP)
    {
      return false;
    }

    if (!call_used_or_fixed_reg_p (regno) || regno == SNOWHOUSECPU_LR)
    {
      if (regno != SNOWHOUSECPU_LR)
      {
        //fprintf (
        //  stderr,
        //  "debugging: !call_used_or_fixed_reg_p (%i)\n",
        //  regno
        //);
      }
      return true;
    }
  }
  return false;
}
// Emit RTL equivalent of ADD3 with the given const_int for
// frame-related registers.
//   op0          - Destination register.
//   op1          - First addendum operand (a register).
//   addendum     - Second addendum operand (a constant).
//   kind         - Note kind.  REG_NOTE_MAX if no note must be added.
// Note: adapted from PRU
static rtx
snowhousecpu_add3_frame_adjust (rtx op0, rtx op1, int addendum,
  const enum reg_note kind)
{
  rtx insn;

  //if (GET_CODE (op0) == REG
  //  && HARD_REGISTER_P (op0))
  //{
  //  fprintf (stderr, "snowhousecpu_add3_frame_adjust: %d\n",
  //    REGNO (op0));
  //}
  //if (GET_CODE (op1) == REG
  //  && HARD_REGISTER_P (op1))
  //{
  //  fprintf (stderr, "...continued: %d\n",
  //    REGNO (op1));
  //}

  rtx op0_adjust = gen_rtx_SET (op0,
    plus_constant (Pmode, op1, addendum));

  insn = emit_insn (op0_adjust);

  /* Attach a note indicating what happened.  */
  if (kind != REG_NOTE_MAX)
  {
    add_reg_note (insn, kind, copy_rtx (op0_adjust));
  }

  RTX_FRAME_RELATED_P (insn) = 1;

  return insn;
}
// Add a const_int to the stack pointer register.
static rtx
snowhousecpu_add_to_sp (int addendum, const enum reg_note kind)
{
  return snowhousecpu_add3_frame_adjust
    (stack_pointer_rtx, stack_pointer_rtx, addendum, kind);
}

static void
snowhousecpu_partial_push (int regno, int idx, bool frame_related_p)
{
  const int addend = (
    //cfun->machine->size_for_adjusting_sp
    //0
    //cfun->machine->local_vars_size
    //+ 
    cfun->machine->outgoing_args_size
  );
  rtx plus, mem, reg;
  rtx_insn* str_insn;
  plus = gen_rtx_PLUS (SImode, stack_pointer_rtx, GEN_INT ((idx * UNITS_PER_WORD) + addend));
  mem = gen_frame_mem (SImode, plus);
  reg = gen_rtx_REG (SImode, regno);
  str_insn = emit_insn (gen_rtx_SET (mem, reg));
  if (frame_related_p)
  {
    RTX_FRAME_RELATED_P (str_insn) = 1;
  }
}
static void
snowhousecpu_partial_pop (int regno, int idx/*, bool frame_related_p*/)
{
  const int addend = (
    //cfun->machine->size_for_adjusting_sp
    //0
    //cfun->machine->local_vars_size
    //+ 
    cfun->machine->outgoing_args_size
  );
  rtx plus, mem, reg;
  rtx_insn* ldr_insn;
  plus = gen_rtx_PLUS (SImode, stack_pointer_rtx, GEN_INT ((idx * UNITS_PER_WORD) + addend));
  mem = gen_frame_mem (SImode, plus);
  reg = gen_rtx_REG (SImode, regno);
  ldr_insn = emit_insn (gen_rtx_SET (reg, mem));
  //if (frame_related_p)
  //{
  //  RTX_FRAME_RELATED_P (ldr_insn) = 1;
  //}
}

static void
snowhousecpu_push (int regno, bool frame_related_p)
{
  //rtx insn, movsi_push, rtx_reg;

  //rtx_reg = gen_rtx_REG (SImode, regno);
  //movsi_push = gen_movsi_push (rtx_reg);
  //insn = emit_insn (movsi_push);

  //// Attach a note indicating what happened.
  //add_reg_note (insn, REG_FRAME_, copy_rtx (rtx_reg));
  //add_reg_note (insn, REG_FRAME_RELATED_EXPR, copy_rtx (movsi_push));

  //RTX_FRAME_RELATED_P (insn) = 1;

  // Idea borrowed from the AVR's `emit_push_byte ()`.
  rtx /*sub,*/ mem, reg;
  rtx_insn* str_insn;
  //rtx_insn* sub_insn;

  ////mem = gen_rtx_POST_DEC (SImode, stack_pointer_rtx);
  ////mem = gen_frame_mem (SImode, mem);
  //sub = gen_rtx_MINUS (SImode, stack_pointer_rtx, GEN_INT (UNITS_PER_WORD));
  ////mem = gen_frame_mem (SImode, mem);
  //sub_insn = emit_insn (gen_rtx_SET (stack_pointer_rtx, sub));

  ////insn = emit_insn (gen_rtx_SET (mem, reg));

  //if (frame_related_p)
  //{
  //  RTX_FRAME_RELATED_P (sub_insn) = 1;
  //}

  //--------
  snowhousecpu_add_to_sp (-UNITS_PER_WORD, REG_NOTE_MAX);
  //--------

  mem = gen_frame_mem (SImode, stack_pointer_rtx);
  reg = gen_rtx_REG (SImode, regno);
  str_insn = emit_insn (gen_rtx_SET (mem, reg));

  if (frame_related_p)
  {
    RTX_FRAME_RELATED_P (str_insn) = 1;
  }

  //return insn;
}

static void//rtx
snowhousecpu_pop (int regno, int idx, bool finish=false)
{
  //rtx insn, movsi_pop, rtx_reg;

  //rtx_reg = gen_rtx_REG (SImode, regno);

  //movsi_pop = gen_movsi_pop (rtx_reg);

  //insn = emit_insn (movsi_pop);

  // Attach a note indicating what happened.
  //add_reg_note (insn, REG_CFA_RESTORE, copy_rtx (rtx_reg));

  //RTX_FRAME_RELATED_P (insn) = 1;

  // idea borrowed from AVR's `emit_pop_byte ()`.
  // Hopefully this will just work.
  //rtx insn, mem, reg;

  //mem = gen_rtx_PRE_INC (SImode, stack_pointer_rtx);
  //mem = gen_frame_mem (SImode, mem);
  //reg = gen_rtx_REG (SImode, regno);

  //--------
  ////insn = emit_insn (gen_rtx_SET (reg, mem));
  //rtx /*add,*/ mem, reg;
  //rtx_insn* ldr_insn;
  ////rtx_insn* add_insn;

  //mem = gen_frame_mem (SImode, stack_pointer_rtx);
  //reg = gen_rtx_REG (SImode, regno);
  //ldr_insn = emit_insn (gen_rtx_SET (reg, mem));

  ////if (frame_related_p)
  ////{
  ////  RTX_FRAME_RELATED_P (ldr_insn) = 1;
  ////}
  //--------

  //add = gen_rtx_PLUS (SImode, stack_pointer_rtx, GEN_INT (UNITS_PER_WORD));
  //add_insn = emit_insn (gen_rtx_SET (stack_pointer_rtx, add));
  if (!finish)
  {
    snowhousecpu_partial_pop (regno, idx);
  }
  else
  {
    snowhousecpu_add_to_sp ((UNITS_PER_WORD * idx), REG_NOTE_MAX);
  }

  //if (frame_related_p)
  //{
  //  RTX_FRAME_RELATED_P (add_insn) = 1;
  //}

  //return insn;
}

//void
//snowhousecpu_expand_prologue ()
//{
//  rtx insn;
//
//  snowhousecpu_compute_frame ();
//
//  if (flag_stack_usage_info)
//  {
//    // this is apparently only used for the user of GCC, rather than the
//    // developer of GCC
//    current_function_static_stack_size
//      = cfun->machine->static_stack_size; /*cfun->machine->size_for_adjusting_sp;*/ 
//  }
//
//  // Save callee-saved registers
//  int idx = 0;
//  for (int regno=0; regno<FIRST_PSEUDO_REGISTER; ++regno)
//  {
//    if (snowhousecpu_regno_actually_callee_saved (regno))
//    {
//      ++idx;
//    }
//  }
//
//  // Adjust the stack pointer
//  if (cfun->machine->size_for_adjusting_sp > 0)
//  {
//    insn = snowhousecpu_add_to_sp (
//      (
//        -(
//          cfun->machine->size_for_adjusting_sp
//          + (idx * UNITS_PER_WORD)
//        )
//      ),
//      REG_FRAME_RELATED_EXPR);
//  }
//  else
//  {
//    // This is for preservring `lr`. For now, we do this even for LEAF functions!
//    insn = snowhousecpu_add_to_sp (
//      -(idx * UNITS_PER_WORD),
//      REG_FRAME_RELATED_EXPR
//    );
//  }
//}
void
snowhousecpu_expand_prologue ()
{
  rtx insn;

  snowhousecpu_compute_frame ();

  if (flag_stack_usage_info)
  {
    // this is apparently only used for the user of GCC, rather than the
    // developer of GCC
    current_function_static_stack_size
      = cfun->machine->static_stack_size; /*cfun->machine->size_for_adjusting_sp;*/ 
  }
  // Save callee-saved registers
  int idx = 0;
  for (int regno=0; regno<FIRST_PSEUDO_REGISTER; ++regno)
  {
    if (snowhousecpu_regno_actually_callee_saved (regno))
    //if (snowhousecpu_regno_actually_callee_saved_no_hfp (regno))
    //if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
    {
      //gen_push (regno);
      ++idx;
    }
  }

  // Adjust the stack pointer.
  //fprintf (
  //  stderr,
  //  "test: %x"
  //);
  if (cfun->machine->size_for_adjusting_sp > 0)
  {
    snowhousecpu_stack_debug_fprintf (
      stderr,
      "debug: size_for_adjusting_sp:%i > 0: "
      "frame_pointer_needed:%u"
      "\n",
      cfun->machine->size_for_adjusting_sp,
      unsigned(frame_pointer_needed)
    );
    insn = snowhousecpu_add_to_sp (
      (
        -(
          cfun->machine->size_for_adjusting_sp
          + (idx * UNITS_PER_WORD)
        )
      ),
      REG_FRAME_RELATED_EXPR);
  }
  else
  {
    snowhousecpu_stack_debug_fprintf (
      stderr,
      "debug: size_for_adjusting_sp:%i <= 0: "
      "frame_pointer_needed:%u"
      "\n",
      cfun->machine->size_for_adjusting_sp,
      unsigned(frame_pointer_needed)
    );
    insn = snowhousecpu_add_to_sp (
      -(idx * UNITS_PER_WORD),
      REG_FRAME_RELATED_EXPR
    );
  }

  // Save the frame pointer
  if (frame_pointer_needed)
  {
    snowhousecpu_push (HARD_FRAME_POINTER_REGNUM, true);
    snowhousecpu_add3_frame_adjust (hard_frame_pointer_rtx, stack_pointer_rtx,
      UNITS_PER_WORD, REG_NOTE_MAX);
  }

  //fprintf (stderr, "\nsnowhousecpu_expand_prologue ()\n");

  // Save callee-saved registers
  idx = 0;
  for (int regno=0; regno<FIRST_PSEUDO_REGISTER; ++regno)
  {
    if (snowhousecpu_regno_actually_callee_saved (regno))
    //if (snowhousecpu_regno_actually_callee_saved_no_hfp (regno))
    //if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
    {
      //gen_push (regno);
      //if (regno == SNOWHOUSECPU_HI) {
      //  fprintf(
      //    stderr,
      //    "eek! 1\n"
      //  );
      //}
      snowhousecpu_partial_push (regno, idx, true);
      ++idx;
    }
  }

  // If we are profiling, make sure no instructions are scheduled before
  // the call to mcount.
  if (crtl->profile)
  {
    emit_insn (gen_blockage ());
  }
}

//--------
void
snowhousecpu_expand_epilogue ()
{
  emit_insn (gen_blockage ());

  snowhousecpu_compute_frame ();

  //if (cfun->machine->size_for_adjusting_sp > 0)
  //{
  //  snowhousecpu_add_to_sp (cfun->machine->size_for_adjusting_sp,
  //    REG_FRAME_RELATED_EXPR);
  //}

  //fprintf (stderr, "\nsnowhousecpu_expand_epilogue ()\n");

  // Restore callee-saved registers
  int idx = 0;
  for (int regno=0; regno<FIRST_PSEUDO_REGISTER; ++regno)
  {
    if (snowhousecpu_regno_actually_callee_saved (regno))
    //if (snowhousecpu_regno_actually_callee_saved_no_hfp (regno))
    //if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
    {
      //gen_pop (regno);
      //fprintf (
      //  stderr,
      //  "restore callee-saved registers: regno:%i idx:%i\n",
      //  regno,
      //  idx
      //);
      snowhousecpu_pop (regno, idx);
      ++idx;
    }
  }

  // Restore the frame pointer.
  //if (df_regs_ever_live_p (SNOWHOUSECPU_FP))
  //if ((cfun->machine->local_vars_size > 0)
  //  || (cfun->machine->stack_args_size > 0))
  if (frame_pointer_needed)
  {
    //gen_pop (SNOWHOUSECPU_FP);
    //gen_pop (HARD_FRAME_POINTER_REGNUM);
    //fprintf (
    //  stderr,
    //  "frame_pointer_needed: regno:%i idx:%i\n",
    //  HARD_FRAME_POINTER_REGNUM,
    //  idx
    //);
    snowhousecpu_pop (HARD_FRAME_POINTER_REGNUM, idx);
    ++idx;
  }
  //snowhousecpu_pop (0, idx, true);
  //if (cfun->machine->size_for_adjusting_sp > 0)
  {
    snowhousecpu_add_to_sp (
      (cfun->machine->size_for_adjusting_sp + idx * UNITS_PER_WORD),
      REG_FRAME_RELATED_EXPR
    );
  }

  emit_jump_insn (gen_returner ());
  //emit_jump_insn (gen_return ());
  //emit_jump_insn (gen_simple_return ());
}
//--------

// BEGIN: OLD `snowhousecpu_expand_epilogue ()`
//void
//snowhousecpu_expand_epilogue ()
//{
//  emit_insn (gen_blockage ());
//
//  //if (cfun->machine->size_for_adjusting_sp > 0)
//  //{
//  //  snowhousecpu_add_to_sp (cfun->machine->size_for_adjusting_sp,
//  //    REG_FRAME_RELATED_EXPR);
//  //}
//
//  //fprintf (stderr, "\nsnowhousecpu_expand_epilogue ()\n");
//
//  // Restore callee-saved registers
//  int idx = 0;
//  for (int regno=0; regno<FIRST_PSEUDO_REGISTER; ++regno)
//  {
//    if (snowhousecpu_regno_actually_callee_saved (regno))
//    //if (snowhousecpu_regno_actually_callee_saved_no_hfp (regno))
//    //if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
//    {
//      //gen_pop (regno);
//      //fprintf (
//      //  stderr,
//      //  "restore callee-saved registers: regno:%i idx:%i\n",
//      //  regno,
//      //  idx
//      //);
//      snowhousecpu_pop (regno, idx);
//      ++idx;
//    }
//  }
//
//  // Restore the frame pointer.
//  //if (df_regs_ever_live_p (SNOWHOUSECPU_FP))
//  //if ((cfun->machine->local_vars_size > 0)
//  //  || (cfun->machine->stack_args_size > 0))
//  if (frame_pointer_needed)
//  {
//    //gen_pop (SNOWHOUSECPU_FP);
//    //gen_pop (HARD_FRAME_POINTER_REGNUM);
//    //fprintf (
//    //  stderr,
//    //  "frame_pointer_needed: regno:%i idx:%i\n",
//    //  HARD_FRAME_POINTER_REGNUM,
//    //  idx
//    //);
//    snowhousecpu_pop (HARD_FRAME_POINTER_REGNUM, idx);
//    ++idx;
//  }
//  //snowhousecpu_pop (0, idx, true);
//  //if (cfun->machine->size_for_adjusting_sp > 0)
//  {
//    snowhousecpu_add_to_sp (
//      (cfun->machine->size_for_adjusting_sp + idx * UNITS_PER_WORD),
//      REG_FRAME_RELATED_EXPR
//    );
//  }
//
//  emit_jump_insn (gen_returner ());
//  //emit_jump_insn (gen_return ());
//  //emit_jump_insn (gen_simple_return ());
//}
// END: OLD `snowhousecpu_expand_epilogue ()`

//extern int which_alternative;

void
snowhousecpu_emit_mov (rtx dst, rtx src, machine_mode mode)
{
  //bool did_inner_print = false;
  rtx operands[2];
  operands[0] = dst;
  operands[1] = src;

  snowhousecpu_debug_fprintf (stderr,
    //"dbg: %s ",
    "mov start: %d:%d:%d:%s\n",
    can_create_pseudo_p (),
    lra_in_progress,
    reload_completed,
    GET_MODE_NAME (mode));
  snowhousecpu_debug_rtx (operands[0]);
  snowhousecpu_debug_rtx (operands[1]);

  if (!(reload_in_progress || reload_completed))
  {
    //if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
    {
      if (
        //can_create_pseudo_p () && 
        MEM_P (operands[0])
      //&& !snowhousecpu_mov_reg_operand (mode, operands[1])
      )
      {
        snowhousecpu_debug_fprintf (stderr, "MEM_P (operands[0])\n");
        {
          operands[1] = force_reg (mode, operands[1]);
          snowhousecpu_debug_fprintf (stderr,
            "operands[1] = force_reg (...):\n");
          snowhousecpu_debug_rtx (operands[1]);
        }

        if (MEM_P (XEXP (operands[0], 0))
          || snowhousecpu_absolute_ex (XEXP (operands[0], 0)))
        {
          rtx tmp;
          snowhousecpu_debug_fprintf (stderr,
            "operands[0] = gen_rtx_MEM (...):\n");
          snowhousecpu_debug_rtx (XEXP (operands[0], 0));
          tmp = force_reg (Pmode, XEXP (operands[0], 0));
          snowhousecpu_debug_rtx (tmp);
          operands[0] = gen_rtx_MEM (mode, tmp);
          snowhousecpu_debug_rtx (operands[0]);
        }
        else
        {
          snowhousecpu_debug_fprintf (stderr,
            "don't have inner `MEM_P`/`snowhousecpu_absolute_ex`\n");
        }
      }
      else if (
        //can_create_pseudo_p () &&
        MEM_P (operands[1])
      )
      {
        snowhousecpu_debug_fprintf (stderr, "MEM_P (operands[1])\n");
        if (MEM_P (XEXP (operands[1], 0))
          || snowhousecpu_absolute_ex (XEXP (operands[1], 0)))
        {
          rtx tmp;
          snowhousecpu_debug_fprintf (stderr,
            "operands[1] = gen_rtx_MEM (...):\n");
          snowhousecpu_debug_rtx (XEXP (operands[1], 0));
          tmp = force_reg (Pmode, XEXP (operands[1], 0));
          snowhousecpu_debug_rtx (tmp);
          operands[1] = gen_rtx_MEM (mode, tmp);
          snowhousecpu_debug_rtx (operands[1]);
        }
        else
        {
          snowhousecpu_debug_fprintf (stderr,
            "don't have inner `MEM_P`/`snowhousecpu_absolute_ex`\n");
        }
      }
    }
    //else if (GET_MODE_SIZE (mode) == UNITS_PER_WORD)
    //{
    //  rtx tmp_dst[2];
    //  rtx tmp_src[2];
    //  if (MEM_P (operands[0]))
    //  {
    //    //operands[1] = force_reg (mode, operands[1]);
    //  }
    //  else if (MEM_P (operands[1]))
    //  {
    //  }
    //  else
    //  {
    //    emit_insn (gen_rtx_SET
    //      (gen_lowpart (SImode, operands[0]),
    //      gen_lowpart (SImode, operands[1])));
    //    emit_insn (gen_rtx_SET
    //      (gen_highpart (SImode, operands[0]),
    //      gen_highpart (SImode, operands[1])));
    //  }
    //}
    //else
    //{
    //  // TODO: figure out how to handle this (if necessary)
    //  gcc_unreachable ();
    //}
    //if (GET_CODE (operands[1]) == SUBREG)
    //{
    //  rtx tmp = simplify_gen_subreg (SImode);
    //}
  }

  snowhousecpu_debug_fprintf (stderr,
    //"dbg: %s ",
    "mov end: %d:%d:%d:%s ",
    can_create_pseudo_p (),
    lra_in_progress,
    reload_completed,
    GET_MODE_NAME (mode));
  snowhousecpu_debug_fprintf (stderr, "\n");
  //if (did_inner_print)
  {
    snowhousecpu_debug_fprintf (stderr, "\n");
  }
  //if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
  {
    //if (can_create_pseudo_p ())
    //{
      emit_insn (gen_rtx_SET (operands[0], operands[1]));
    //  emit_move_insn (operands[0], operands[1]);
    //}
    //else
    //{
    //  emit_move_insn_1 (operands[0], operands[1]);
    //}
    //emit_move_insn (operands[0], operands[1]);
  }
  //else if (GET_MODE_SIZE (mode) == UNITS_PER_WORD * 2)
  //{
  //  emit_insn (gen_rtx_SET
  //    (gen_lowpart (SImode, operands[0]),
  //    gen_lowpart (SImode, operands[1])));
  //  emit_insn (gen_rtx_SET
  //    (gen_highpart (SImode, operands[0]),
  //    gen_highpart (SImode, operands[1])));
  //}
  //else
  //{
  //  gcc_unreachable ();
  //}
}
// Implement RETURN_ADDR_RTX.  Note, we do not support moving
// back to a previous frame.
rtx
snowhousecpu_get_return_address (int count)
{
  if (count != 0)
  {
    return NULL_RTX;
  }

  return get_hard_reg_initial_val (Pmode, SNOWHOUSECPU_LR);
}
static void
snowhousecpu_asm_named_section (const char *name, unsigned int flags, tree decl)
{
  default_elf_asm_named_section (name, flags, decl);
}

//static bool
//snowhousecpu_hard_regno_mode_ok (unsigned int regno, machine_mode mode)
//{
//  if (GET_MODE_CLASS (mode) == MODE_CC)
//  {
//    //return (regno == SNOWHOUSECPU_FLAGS);
//    return (regno <= SNOWHOUSECPU_LAST_GENERAL_REGNUM);
//  }
//  //if (regno == SNOWHOUSECPU_FLAGS && GET_MODE_CLASS (mode) != MODE_CC)
//  //{
//  //  return false;
//  //}
//
//  if (regno <= SNOWHOUSECPU_LR
//    && GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
//  {
//    return true;
//  }
//  if (regno == HARD_FRAME_POINTER_REGNUM || regno == STACK_POINTER_REGNUM
//    || regno == FRAME_POINTER_REGNUM || regno == ARG_POINTER_REGNUM)
//  {
//    return (GET_MODE_SIZE (mode) <= UNITS_PER_WORD);
//  }
//
//  if (regno < SNOWHOUSECPU_LR
//    && GET_MODE_SIZE (mode) > UNITS_PER_WORD)
//  {
//    const unsigned int
//      num_consec_regs = ((GET_MODE_SIZE (mode)) / UNITS_PER_WORD);
//
//    if ((regno % num_consec_regs) == 0)
//    {
//      return true;
//    }
//  }
//  return false;
//}
//static void
//snowhousecpu_setup_incoming_varargs
//  (cumulative_args_t ca_v,
//  const function_arg_info& arg ATTRIBUTE_UNUSED,
//  int *pretend_args_size, int no_rtl)
//{
//  CUMULATIVE_ARGS* ca = get_cumulative_args (ca_v);
//  int nregs = SNOWHOUSECPU_NUM_ARG_REGS - (*ca);
//  
//  *pretend_args_size = (nregs < 0) ? 0 : (UNITS_PER_WORD * nregs);
//  
//  if (!no_rtl)
//  {
//    for (int other_regno=*ca; other_regno<SNOWHOUSECPU_NUM_ARG_REGS; ++other_regno)
//    {
//      int regno = other_regno + SNOWHOUSECPU_FIRST_ARG_REGNUM;
//      rtx reg = gen_rtx_REG (SImode, regno);
//      rtx slot = gen_rtx_PLUS
//        (Pmode,
//        gen_rtx_REG (SImode, ARG_POINTER_REGNUM),
//        GEN_INT (UNITS_PER_WORD * (regno - 1)));
//        //rtx slot = gen_rtx_PLUS (Pmode,
//        //                      gen_rtx_REG (SImode, ARG_POINTER_REGNUM),
//        //                      GEN_INT (UNITS_PER_WORD * regno /*(3 + (regno-2))*/));
//      
//      emit_move_insn (gen_rtx_MEM (SImode, slot), reg);
//    }
//    //return;
//  }
//  if (REG_PARM_STACK_SPACE (cfun->decl) == 0)
//  {
//    cfun->machine->varargs_extra_stack_size = nregs * UNITS_PER_WORD;
//  }
//}
//static void
//snowhousecpu_emit_compare (enum rtx_code *code, rtx *op0, rtx *op1)
//{
//  rtx cmp_op0 = *op0;
//  rtx cmp_op1 = *op1;
//  if (GET_MODE_CLASS (GET_MODE (*op0)) == MODE_INT)
//  {
//    if (*code == EQ)
//    {
//    }
//    else if (*code == NE)
//    {
//    }
//    else if (*code == GEU)
//    {
//    }
//    else if (*code == LTU)
//    {
//    }
//    else if (*code == GTU)
//    {
//    }
//    else if (*code == GEU)
//    {
//    }
//  }
//}

//const char*
//snowhousecpu_expand_conditional_branch (rtx *operands)
//{
//  enum rtx_code code = GET_CODE (operands[0]);
//  rtx op0 = operands[1];
//  rtx op1 = operands[2];
//  rtx label = operands[3];
//  static constexpr size_t STR_SIZE = 512;
//  static char my_str[STR_SIZE];
//  (void)memset(my_str, sizeof(my_str[0]) * STR_SIZE, 0);
//  if (code == EQ)
//  {
//    snprintf(my_str, STR_SIZE, "beq r%i, r%i, ");
//  }
//  else if (code == NE)
//  {
//  }
//  else if (code == GEU)
//  {
//  }
//  else if (code == LTU)
//  {
//  }
//  else if (code == GTU)
//  {
//  }
//  else if (code == GEU)
//  {
//  }
//  else if (code == GE)
//  {
//  }
//  else if (code == LT)
//  {
//  }
//  else if (code == GT)
//  {
//  }
//  else //if (code == GE)
//  {
//  }
//}
//--------
//void
//snowhousecpu_gen_cmpdi_reg (rtx x, rtx y)
//{
//  //machine_mode mode;
//  //rtx cc_reg;
//
//  rtx x_lo
//    = simplify_gen_subreg (SImode, x, DImode,
//      subreg_lowpart_offset (SImode, DImode));
//  rtx x_hi
//    = simplify_gen_subreg (SImode, x, DImode,
//      subreg_highpart_offset (SImode, DImode));
//  rtx y_lo
//    = simplify_gen_subreg (SImode, y, DImode,
//      subreg_lowpart_offset (SImode, DImode));
//  rtx y_hi
//    = simplify_gen_subreg (SImode, y, DImode,
//      subreg_highpart_offset (SImode, DImode));
//  rtx scratch = gen_reg_rtx (SImode);
//  emit_insn (gen_cmpsi (x_lo, y_lo));
//  emit_insn (gen_snowhousecpu_cmpsi3_carry_in (scratch, x_hi, y_hi));
//}
//--------
//#undef COMPAT_SIMODE_TRAPPING_ARITHMETIC
//#undef L_addvsi3
//#undef L_addvdi3
//
//#undef L_subvsi3
//#undef L_subvdi3
//
//#undef L_mulvsi3
//#undef L_mulvdi3
//
//#undef L_divvsi3
//#undef L_divvdi3
//
//#undef L_udivvsi3
//#undef L_udivvdi3
//
//#undef L_ffsdi2
//--------
//unsigned char
//snowhousecpu_target_class_max_nregs (reg_class_t rclass, machine_mode mode)
//{
//  if (
//    mode == DImode
//    && rclass == DIVW_OUTP_REGS
//  )
//  {
//    return 1ull;
//  }
//  else if (GET_MODE_SIZE (mode) <= GET_MODE_SIZE (SImode))
//  {
//    return GET_MODE_SIZE (SImode);
//  }
//  else
//  {
//    return (GET_MODE_SIZE (mode) * UNITS_PER_WORD);
//  }
//}

//reg_class_t
//snowhousecpu_spill_class (reg_class_t rclass, machine_mode mode)
//{
//  if (rclass == DIVW_OUTP_REGS)
//  {
//    return GENERAL_REGS;
//  }
//  else
//  {
//    return NO_REGS;
//  }
//}
bool
snowhousecpu_push_argument (unsigned int npush)
{
  return false;
}

//long long
//snowhousecpu_stack_pointer_offset ()
//{
//  struct machine_function machine;
//  memset(&machine, 0, sizeof(machine));
//  snowhousecpu_compute_frame (&machine);
//  using my_long_long_t = long long int;
//  return machine.size_for_adjusting_sp - my_long_long_t(machine.outgoing_args_size);
//}

const char *snowhousecpu_asm_byte_op = "\t.byte\t";

const char *snowhousecpu_asm_aligned_hi_op = "\t.i16\t";
const char *snowhousecpu_asm_aligned_psi_op = NULL;
const char *snowhousecpu_asm_aligned_si_op = "\t.i32\t";
const char *snowhousecpu_asm_aligned_pdi_op = NULL;
const char *snowhousecpu_asm_aligned_di_op = "\t.i64\t";
const char *snowhousecpu_asm_aligned_pti_op = NULL;
const char *snowhousecpu_asm_aligned_ti_op = NULL;

const char *snowhousecpu_asm_unaligned_hi_op = NULL;
const char *snowhousecpu_asm_unaligned_psi_op = NULL;
const char *snowhousecpu_asm_unaligned_si_op = NULL;
const char *snowhousecpu_asm_unaligned_pdi_op = NULL;
const char *snowhousecpu_asm_unaligned_di_op = NULL;
const char *snowhousecpu_asm_unaligned_pti_op = NULL;
const char *snowhousecpu_asm_unaligned_ti_op = NULL;
// --------
// Initialize the GCC target structure.
//#undef TARGET_CLASS_MAX_NREGS
//#define TARGET_CLASS_MAX_NREGS snowhousecpu_target_class_max_nregs

//#undef TARGET_SPILL_CLASS
//#define TARGET_SPILL_CLASS snowhousecpu_spill_class
#undef TARGET_PUSH_ARGUMENT
#define TARGET_PUSH_ARGUMENT snowhousecpu_push_argument

#undef TARGET_ASM_BYTE_OP
#define TARGET_ASM_BYTE_OP snowhousecpu_asm_byte_op

#undef TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP snowhousecpu_asm_aligned_hi_op
#undef TARGET_ASM_ALIGNED_PSI_OP
#define TARGET_ASM_ALIGNED_PSI_OP snowhousecpu_asm_aligned_psi_op
#undef TARGET_ASM_ALIGNED_SI_OP
#define TARGET_ASM_ALIGNED_SI_OP snowhousecpu_asm_aligned_si_op
#undef TARGET_ASM_ALIGNED_PDI_OP
#define TARGET_ASM_ALIGNED_PDI_OP snowhousecpu_asm_aligned_pdi_op
#undef TARGET_ASM_ALIGNED_DI_OP
#define TARGET_ASM_ALIGNED_DI_OP snowhousecpu_asm_aligned_di_op
#undef TARGET_ASM_ALIGNED_PTI_OP
#define TARGET_ASM_ALIGNED_PTI_OP snowhousecpu_asm_aligned_pti_op
#undef TARGET_ASM_ALIGNED_TI_OP
#define TARGET_ASM_ALIGNED_TI_OP snowhousecpu_asm_aligned_ti_op

#undef TARGET_ASM_UNALIGNED_HI_OP
#define TARGET_ASM_UNALIGNED_HI_OP snowhousecpu_asm_unaligned_hi_op
#undef TARGET_ASM_UNALIGNED_PSI_OP
#define TARGET_ASM_UNALIGNED_PSI_OP snowhousecpu_asm_unaligned_psi_op
#undef TARGET_ASM_UNALIGNED_SI_OP
#define TARGET_ASM_UNALIGNED_SI_OP snowhousecpu_asm_unaligned_si_op
#undef TARGET_ASM_UNALIGNED_PDI_OP
#define TARGET_ASM_UNALIGNED_PDI_OP snowhousecpu_asm_unaligned_pdi_op
#undef TARGET_ASM_UNALIGNED_DI_OP
#define TARGET_ASM_UNALIGNED_DI_OP snowhousecpu_asm_unaligned_di_op
#undef TARGET_ASM_UNALIGNED_PTI_OP
#define TARGET_ASM_UNALIGNED_PTI_OP snowhousecpu_asm_unaligned_pti_op
#undef TARGET_ASM_UNALIGNED_TI_OP
#define TARGET_ASM_UNALIGNED_TI_OP snowhousecpu_asm_unaligned_ti_op

#undef TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES hook_bool_const_tree_true

#undef TARGET_HAVE_LIBATOMIC
#define TARGET_HAVE_LIBATOMIC false//true
//#undef TARGET_PROMOTE_FUNCTION_MODE
//#define TARGET_PROMOTE_FUNCTION_MODE
//  default_promote_function_mode_always_promote

#undef TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY snowhousecpu_return_in_memory
#undef TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK must_pass_in_stack_var_size
//#undef TARGET_MUST_PASS_IN_STACK
//#define TARGET_MUST_PASS_IN_STACK snowhousecpu_must_pass_in_stack
//#define TARGET_MUST_PASS_IN_STACK must_pass_in_stack_var_size
#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE snowhousecpu_pass_by_reference
#undef  TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES snowhousecpu_arg_partial_bytes
//#define TARGET_ARG_PARTIAL_BYTES hook_bool_void_false
#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG snowhousecpu_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE snowhousecpu_function_arg_advance

#undef TARGET_LRA_P
//#define TARGET_LRA_P hook_bool_void_false
#define TARGET_LRA_P hook_bool_void_true

#undef  TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P snowhousecpu_legitimate_address_p

//#undef  TARGET_SETUP_INCOMING_VARARGS
//#define TARGET_SETUP_INCOMING_VARARGS snowhousecpu_setup_incoming_varargs
#undef TARGET_STRICT_ARGUMENT_NAMING
#define TARGET_STRICT_ARGUMENT_NAMING hook_bool_CUMULATIVE_ARGS_true

//#undef TARGET_SECONDARY_RELOAD
//#define TARGET_SECONDARY_RELOAD snowhousecpu_secondary_reload

//#undef TARGET_FIXED_CONDITION_CODE_REGS
//#define TARGET_FIXED_CONDITION_CODE_REGS snowhousecpu_fixed_condition_code_regs

//#define TARGET_SPILL_CLASS snowhousecpu_target_spill_class

// Define this to return an RTX representing the place where a
// function returns or receives a value of data type RET_TYPE, a tree
// node representing a data type.
#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE snowhousecpu_function_value
#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE snowhousecpu_libcall_value
#undef TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P snowhousecpu_function_value_regno_p

#undef TARGET_FRAME_POINTER_REQUIRED
//#define TARGET_FRAME_POINTER_REQUIRED hook_bool_void_true
#define TARGET_FRAME_POINTER_REQUIRED hook_bool_void_false

//#undef TARGET_STATIC_CHAIN
//#define TARGET_STATIC_CHAIN snowhousecpu_static_chain

//#undef TARGET_ASM_TRAMPOLINE_TEMPLATE
//#define TARGET_ASM_TRAMPOLINE_TEMPLATE snowhousecpu_asm_trampoline_template
//#undef TARGET_TRAMPOLINE_INIT
//#define TARGET_TRAMPOLINE_INIT snowhousecpu_trampoline_init

//#undef TARGET_HARD_REGNO_MODE_OK 
//#define TARGET_HARD_REGNO_MODE_OK snowhousecpu_hard_regno_mode_ok

#undef TARGET_ASM_NAMED_SECTION 
#define TARGET_ASM_NAMED_SECTION snowhousecpu_asm_named_section

//#undef TARGET_RTX_COSTS
//#define TARGET_RTX_COSTS snowhousecpu_target_rtx_costs
//
//#undef TARGET_MEMORY_MOVE_COST
//#define TARGET_MEMORY_MOVE_COST snowhousecpu_target_memory_move_cost

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE snowhousecpu_option_override

#undef  TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND snowhousecpu_print_operand
#undef  TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS snowhousecpu_print_operand_address

#undef  TARGET_CONSTANT_ALIGNMENT
#define TARGET_CONSTANT_ALIGNMENT constant_alignment_word_strings

//#undef TARGET_PREFERRED_RELOAD_CLASS
//#define TARGET_PREFERRED_RELOAD_CLASS snowhousecpu_preferred_reload_class
//#undef TARGET_ASM_UNIQUE_SECTION
//#define TARGET_ASM_UNIQUE_SECTION snowhousecpu_unique_section

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-snowhousecpu.h"
