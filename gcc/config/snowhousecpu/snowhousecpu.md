;; Machine description for SnowHouseCpu
;; Copyright (C) 2025 Free Software Foundation, Inc.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.


(include "iterators.md")
(include "constants.md")
(include "constraints.md")
(include "predicates.md")
(include "atomic.md")



;; Most instructions are two bytes long
;;(define_attr "length" "" (const_int 2))

(define_insn "nop"
  [(const_int 0)]
  "true"
  "cpy r0, r0    // nop")

;; -----------------------
;; Arithmetic Instructions
;; -----------------------
(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (plus:SI
      (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  add %0, %1, %2  // addsi3: =r, r, r
  add %0, %1, %2  // addsi3: =r, r, i"
)
(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (minus:SI
      (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  sub %0, %1, %2  // subsi3: =r, r, r
  add %0, %1, -%2  // subsi3: =r, r, i"
)


;;(define_insn "mulsi3"
;;  [(parallel [
;;    (set (match_operand:SI 0 "register_operand" "=r")
;;        (mult:SI
;;        (match_operand:SI 1 "register_operand" "r")
;;        (match_operand:SI 2 "register_operand" "r")))
;;    (clobber (reg:SI REG_HI))
;;  ])]
;;  ""
;;  "umulw %0, %1, %2"
;;)
(define_expand "umulsidi3"
  [(set (match_operand:DI 0 "register_operand")
    (mult:DI
      (zero_extend:DI (match_operand:SI 1 "register_operand"))
      (zero_extend:DI (match_operand:SI 2 "register_operand"))))]
  ""
  {
    emit_insn (gen_umulw
      (gen_lowpart (SImode, operands[0]),
      gen_highpart (SImode, operands[0]),
      operands[1], operands[2]));
    DONE;
  }
)
(define_expand "smulsidi3"
  [(set (match_operand:DI 0 "register_operand")
    (mult:DI
      (sign_extend:DI (match_operand:SI 1 "register_operand"))
      (sign_extend:DI (match_operand:SI 2 "register_operand"))))]
  ""
  {
    emit_insn (gen_smulw
      (gen_lowpart (SImode, operands[0]),
      gen_highpart (SImode, operands[0]),
      operands[1], operands[2]));
    DONE;
  }
)
(define_insn "umulw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (mult:SI
         (match_operand:SI 2 "register_operand" "r")
         (match_operand:SI 3 "register_operand" "r")))
   (set (match_operand:SI 1 "register_operand" "=w")
        (truncate:SI
         (lshiftrt:DI
          (mult:DI (zero_extend:DI (match_dup 2)) (zero_extend:DI (match_dup 3)))
          (const_int 32))))]
  ""
  "umulw %0, %2, %3 // %2 * %3 => {%1, %0} "
)
(define_insn "smulw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (mult:SI
         (match_operand:SI 2 "register_operand" "r")
         (match_operand:SI 3 "register_operand" "r")))
   (set (match_operand:SI 1 "register_operand" "=w")
        (truncate:SI
         (lshiftrt:DI
          (mult:DI (sign_extend:DI (match_dup 2)) (sign_extend:DI (match_dup 3)))
          (const_int 32))))]
  ""
  "smulw %0, %2, %3 // // %2 * %3 => {%1, %0}"
)
;;--------
(define_insn "udivdi3"
  [(set (match_operand:DI 0 "register_operand" "=h")
    (udiv:DI
      (match_operand:DI 1 "register_operand" "0")
      ;;(match_dup 0) ;; can't use this for duplicate registers!
      (match_operand:DI 2 "register_operand" "r")))]
  ""
  "udivw %L0, %H2, %L2 // {%H1, %L1} / {%H2, %L2} => {%H0, %L0}"
  ;;"udivw %L0, %H1, %L1 // %L0 %H0 %L1 %H1"
)
;;(define_insn "divdi3"
;;  [(set (match_operand:DI 0 "register_operand" "=h")
;;    (div:DI
;;      (match_operand:DI 1 "register_operand" "0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "sdivw %L0, %H2, %L2 // %L0 %H0 %L1 %H1 %L2 %H2"
;;)
(define_insn "divdi3"
  [(set (match_operand:DI 0 "register_operand" "=h")
    (div:DI
      (match_operand:DI 1 "register_operand" "0")
      ;;(match_dup 0) ;; can't use this for duplicate registers!
      (match_operand:DI 2 "register_operand" "r")))]
  ""
  "sdivw %L0, %H2, %L2 // {%H1, %L1} / {%H2, %L2} => {%H0, %L0}"
)

;;(define_expand "udivdi3"
;;  [(set (match_operand:DI 0 "register_operand")
;;    (udiv:DI
;;      (match_operand:DI 1 "register_operand")
;;      (match_operand:DI 2 "register_operand")))]
;;  ""
;;  {
;;    emit_insn (gen_udivw
;;      (gen_lowpart (SImode, operands[0]),
;;      gen_highpart (SImode, operands[0]),
;;      operands[1], operands[2]));
;;    DONE;
;;  }
;;)
;;(define_expand "divdi3"
;;  [(set (match_operand:DI 0 "register_operand")
;;    (div:DI
;;      (match_operand:DI 1 "register_operand")
;;      (match_operand:DI 2 "register_operand")))]
;;  ""
;;  {
;;    emit_insn (gen_sdivw
;;      (gen_lowpart (SImode, operands[0]),
;;      gen_highpart (SImode, operands[0]),
;;      operands[1], operands[2]));
;;    DONE;
;;  }
;;)
;;(define_insn "udivw"
;;  [(set (match_operand:SI 0 "register_operand" "=r")
;;        (truncate:SI
;;          (udiv:DI
;;            (match_dup 0)
;;            (match_operand:DI 2 "register_operand" "r")))
;;   (set (match_operand:SI 1 "register_operand" "=w")
;;        (truncate:SI
;;         (lshiftrt:DI
;;          (udiv:DI (match_dup 2) (match_dup 3))
;;          (const_int 32))))]
;;  ""
;;  "udivw\t\t%0, %2, %3"
;;)
;;(define_insn "sdivw"
;;  [(set (match_operand:SI 0 "register_operand" "=r")
;;        (div:SI
;;         (match_operand:SI 2 "register_operand" "r")
;;         (match_operand:SI 3 "register_operand" "r")))
;;   (set (match_operand:SI 1 "register_operand" "=w")
;;        (truncate:SI
;;         (lshiftrt:DI
;;          (div:DI (sign_extend:DI (match_dup 2)) (sign_extend:DI (match_dup 3)))
;;          (const_int 32))))]
;;  ""
;;  "sdivw\t\t%1, %2, %3"
;;)

;; --------
;; TODO: come back to this when more multiply/divide instructions exist in SnowHouseCpu
;;(define_expand "<Us>mulsidi3"
;;  [(set (match_operand:DI 0 "register_operand")
;;    (mult:DI
;;      (EXTEND:DI (match_operand:SI 1 "register_operand"))
;;      (EXTEND:DI (match_operand:SI 2 "register_operand"))))]
;;  ""
;;{
;;  emit_insn (gen_l<US>mul (gen_lowpart (SImode, operands[0]),
;;    gen_highpart (SImode, operands[0]),
;;    operands[1], operands[2]));
;;  DONE;
;;})
;;(define_expand "<Us>mulsi3_highpart"
;;  [(parallel
;;    [(set (match_operand:SI 0 "register_operand")
;;      (truncate:SI
;;        (lshiftrt:DI
;;          (mult:DI
;;            (EXTEND:DI (match_operand:SI 1 "register_operand"))
;;            (EXTEND:DI (match_operand:SI 2 "register_operand")))
;;          (const_int 32))))
;;      (clobber (match_operand:SI 3 "register_operand"))])]
;;  ""
;;  ""
;;)
;;
;;(define_insn "*l<US>mul_high"
;;  [(set (match_operand:SI 0 "snowhousecpu_full_product_high_reg" "=x")
;;    (truncate:SI
;;      (lshiftrt:DI
;;        (mult:DI
;;          (EXTEND:DI (match_operand:SI 1 "register_operand" "r"))
;;          (EXTEND:DI (match_operand:SI 2 "register_operand" "r")))
;;        (const_int 32))))
;;    (clobber (match_operand:SI 3 "snowhousecpu_full_product_low_reg" "=y"))]
;;  ""
;;  "l<US>mul %1, %2    // *l<US>mul_high"
;;)
;;(define_insn "l<US>mul"
;;  [(set (match_operand:SI 0 "snowhousecpu_full_product_high_reg" "=x")
;;    (mult:SI
;;      (match_operand:SI 2 "register_operand" "r")
;;      (match_operand:SI 3 "register_operand" "r")))
;;  (set (match_operand:SI 1 "snowhousecpu_full_product_low_reg" "=y")
;;    (truncate:SI
;;      (lshiftrt:DI
;;        (mult:DI (EXTEND:DI (match_dup 2)) (EXTEND:DI (match_dup 3)))
;;        (const_int 32))))]
;;  ""
;;  "l<US>mul %1, %2    // <US>mul"
;;)

;; --------
(define_insn "udivsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (udiv:SI
      (match_operand:SI 1 "register_operand" "r")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "udiv %0, %1, %2")

(define_insn "divsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (div:SI
      (match_operand:SI 1 "register_operand" "r")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "sdiv %0, %1, %2")

(define_insn "umodsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (umod:SI
      (match_operand:SI 1 "register_operand" "r")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "umod %0, %1, %2")

(define_insn "modsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (mod:SI
      (match_operand:SI 1 "register_operand" "r")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "smod %0, %1, %2")

;; --------
;;(define_expand "udivdi3"
;;  [(set (match_operand:DI 0 "register_operand" "")
;;    (udiv:DI
;;      (match_operand:DI 1 "register_operand" "")
;;      (match_operand:DI 2 "register_operand" "")))]
;;  ""
;;{
;;  if (REGNO (operands[0]) != FLARE32_R0)
;;  {
;;    auto tmp_r0 = gen_rtx_REG (DImode, FLARE32_R0);
;;    emit_insn (gen_movdi (tmp_r0, operands[1]));
;;    emit_insn (gen_udivdi3_raw (tmp_r0, tmp_r0, operands[2]));
;;    emit_insn (gen_movdi (operands[0], tmp_r0));
;;    emit_insn (gen_blockage ());
;;    DONE;
;;  }
;;  else
;;  {
;;    emit_insn (gen_udivdi3_raw (operands[0], operands[1], operands[2]));
;;    DONE;
;;  }
;;  emit_insn (gen_udivdi3_raw (operands[0], operands[1], operands[2]));
;;})
;;(define_insn "udivdi3_raw"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;    (udiv:DI
;;      (match_operand:DI 1 "register_operand" "%0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "udiv64 %H2, %L2"
;;)

;;;; Use this one
;;(define_insn "udivdi3"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;    (udiv:DI
;;      (match_operand:DI 1 "register_operand" "0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "udiv64 %H0, %H2"
;;)
;;;; Use this one
;;(define_insn "divdi3"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;    (div:DI
;;      (match_operand:DI 1 "register_operand" "0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "sdiv64 %H0, %H2"
;;)
;;
;;;; Use this one
;;(define_insn "umoddi3"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;    (umod:DI
;;      (match_operand:DI 1 "register_operand" "0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "umod64 %H0, %H2"
;;)
;;
;;;; Use this one
;;(define_insn "moddi3"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;    (mod:DI
;;      (match_operand:DI 1 "register_operand" "0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "lmod %H0, %L0"
;;)

;; --------
;;(define_insn "mulsi3"
;;  [(set (match_operand:SI 0 "register_operand" "=l")
;;    (mult:SI
;;      (match_operand:SI 1 "register_operand" "h")
;;      (match_operand:SI 2 "register_operand" "l")))
;;    (clobber (match_operand:SI 3 "register_operand" ""))]
;;  ""
;;{
;;  //if (REGNO_REG_CLASS (REGNO (operands[1])) != HI_REGS)
;;  //{
;;  //  auto hi = gen_rtx_REG (SImode, REG_HI);
;;  //  emit_insn (gen_movsi (operands[3], hi));
;;
;;  //}
;;
;;  //else
;;  //{
;;  //  return "lumul";
;;  //}
;;  emit_insn (gen_movsi (operands[3], operands[1]));
;;  return "lumul\ncpyspec hi, %3";
;;})


(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
  (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
  (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  lsl %0, %1, %2
  lsl %0, %1, %2"
)

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  lsr %0, %1, %2
  lsr %0, %1, %2"
)
;;(define_insn "lshrhi3"
;;  [(set (match_operand:HI 0 "register_operand" "=r")
;;    (lshiftrt:HI (match_operand:HI 1 "register_operand" "0")
;;      (match_operand:SI 2 "register_operand" "r")))]
;;  ""
;;  "lsrh %0, %2")
;;(define_insn "lshrqi3"
;;  [(set (match_operand:QI 0 "register_operand" "=r")
;;    (lshiftrt:QI (match_operand:QI 1 "register_operand" "0")
;;      (match_operand:SI 2 "register_operand" "r")))]
;;  ""
;;  "lsrb %0, %2")

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  asr %0, %1, %2
  asr %0, %1, %2"
)
;;(define_insn "ashrhi3"
;;  [(set (match_operand:HI 0 "register_operand" "=r")
;;    (ashiftrt:HI (match_operand:HI 1 "register_operand" "0")
;;      (match_operand:SI 2 "register_operand" "r")))]
;;  ""
;;  "asrh %0, %2")
;;(define_insn "ashrqi3"
;;  [(set (match_operand:QI 0 "register_operand" "=r")
;;    (ashiftrt:QI (match_operand:QI 1 "register_operand" "0")
;;      (match_operand:SI 2 "register_operand" "r")))]
;;  ""
;;  "asrb %0, %2")

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (and:SI (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  and %0, %1, %2
  and %0, %1, %2"
)

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (ior:SI (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  or %0, %1, %2
  or %0, %1, %2"
)

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (xor:SI (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  xor %0, %1, %2
  xor %0, %1, %2"
)

(define_expand "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (not:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
{
  auto tmp_const = GEN_INT (-1);
  emit_insn (gen_xorsi3 (operands[0], operands[1], tmp_const));
  DONE;
})


;; Push a register onto the stack
;;(define_insn "movsi_push"
;;  [(set (mem:SI (post_dec:SI (reg:SI REG_SP)))
;;    (match_operand:SI 0 "register_operand" "r"))]
;;  ""
;;  ;;"push %0"
;;  "str(%0, sp, 0),\n  sub(sp, sp, 4),\n"
;;)
;;(define_insn "pushsi1"
;;  [(set (match_operand:SI 0 "register_operand" "r"))]
;;  ""
;;  ;;"push %0"
;;  "str(%0, sp, 0),\n  sub(sp, sp, 4),\n"
;;)

;; Pop a register from the stack
;;(define_insn "popsi1"
;;  [(pop (match_operand:SI 0 "register_operand" "=r"))]
;;  ""
;;  ;;"pop %0"
;;  "add(sp, sp, 4),\n  ldr(%0, sp, 0),"
;;)
;;(define_insn "popsi1"
;;  [(set (match_operand:SI 0 "register_operand" "=r")
;;    (mem:SI (pre_inc:SI (reg:SI REG_SP))))]
;;  ""
;;  ;;"pop %0"
;;  "add(sp, sp, 4),\n  ldr(%0, sp, 0),"
;;)


(define_expand "mov<mode>"
  [(set (match_operand:MOV32 0 "nonimmediate_operand" "")
    (match_operand:MOV32 1
      "snowhousecpu_general_mov_src_operand"
      ""))]
  ""
{
  snowhousecpu_emit_mov (operands[0], operands[1], <MODE>mode);
  DONE;
})


;;(define_insn "*mov32"
;;  [(set (match_operand:MOV32 0
;;    "nonimmediate_operand"
;;    "=r,r,r,B,r,W,r"
;;    )
;;    (match_operand:MOV32 1 
;;      "snowhousecpu_general_mov32_src_operand"
;;      "r,i,B,r,W,r,d"
;;      ))]
;;
;;  "register_operand (operands[0], <MODE>mode)
;;    || register_operand (operands[1], <MODE>mode)"
;;  ;;""
;;  "@
;;  cpy %0, %1    // *mov32: =r, r
;;  cpy %0, #%1    // *mov32: =r, i
;;  ldr %0, [%1]    // *mov32: =r, B
;;  str %1, [%0]    // *mov32: =B, r
;;  ldr %0, [%1]    // *mov32: =r, W
;;  str %1, [%0]    // *mov32: =W, r
;;  add %0, %1, #0x0    // *mov32: =r, d"
;;
;;  ;;cpy %0, %1    // *mov32: =r, S
;;  ;;cpy %0, %1    // *mov32: =S, r
;;  ;;cpy %0, %1    // *mov32: =S, S
;;)
(define_insn "*mov32"
  [(set (match_operand:MOV32 0
    "nonimmediate_operand"
    "=r,r,h,r,r,B,r"
    )
    (match_operand:MOV32 1 
      "snowhousecpu_general_mov_src_operand"
      "r,h,r,i,B,r,d"
      ))]

  "register_operand (operands[0], <MODE>mode)
    || register_operand (operands[1], <MODE>mode)"
  ;;""
  "@
  cpy %0, %1        // *mov32: =r, r
  cpy %0, %1        // *mov32: =r, h
  cpy %0, %1        // *mov32: =h, r
  cpy %0, %1        // *mov32: =r, i
  ldr %0, %1        // *mov32: =r, B
  str %1, %0        // *mov32: =B, r
  add %0, %1, 0x0   // *mov32: =r, d"
)

(define_insn "*sltusi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (ltu:SI
      (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  sltu %0, %1, %2   // sltsi3: =r, r, r
  sltu %0, %1, %2   // sltsi3: =r, r, i"
)
(define_insn "*sltsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (lt:SI
      (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  slts %0, %1, %2   // sltsi3: =r, r, r
  slts %0, %1, %2   // sltsi3: =r, r, i"
)



;; --------
(define_expand "mov<mode>"
  [(set (match_operand:MOV64 0 "register_operand" "=r,r,h")
    (match_operand:MOV64 1
      "register_operand" "r,h,r"))]
  "reload_completed"
{
  //snowhousecpu_emit_mov (operands[0], operands[1], <MODE>mode);
  //debug_rtx (operands[0]);
  //debug_rtx (operands[1]);
  rtx& dst = operands[0];
  rtx& src = operands[1];
  rtx dst_lo
    = simplify_gen_subreg (SImode, dst, <MODE>mode,
      subreg_lowpart_offset (SImode, <MODE>mode));
  rtx dst_hi
    = simplify_gen_subreg (SImode, dst, <MODE>mode,
      subreg_highpart_offset (SImode, <MODE>mode));
  rtx src_lo
    = simplify_gen_subreg (SImode, src, <MODE>mode,
      subreg_lowpart_offset (SImode, <MODE>mode));
  rtx src_hi
    = simplify_gen_subreg (SImode, src, <MODE>mode,
      subreg_highpart_offset (SImode, <MODE>mode));

  snowhousecpu_emit_mov (dst_lo, src_lo, SImode);
  snowhousecpu_emit_mov (dst_hi, src_hi, SImode);

  //snowhousecpu_emit_mov (
  //  (gen_lowpart (SImode, operands[0])),
  //  (gen_lowpart (SImode, operands[1])),
  //  //<MODE>mode
  //  SImode
  //);
  //snowhousecpu_emit_mov (
  //  (gen_highpart (SImode, operands[0])),
  //  (gen_highpart (SImode, operands[1])),
  //  //<MODE>mode
  //  SImode
  //);
  DONE;
})
;;(define_insn "*mov64"
;;  [(set (match_operand:MOV64 0
;;    "register_operand"
;;    "=r,r,h")
;;    (match_operand:MOV64 1
;;    "register_operand"
;;    "r,h,r"))]
;;  ""
;;  "@
;;  cpy %L0, %L1\n\tcpy %H0, %H1  // mov64: =r, r
;;  cpy %L0, %L1\n\tcpy %H0, %H1  // mov64: =r, h
;;  cpy %L0, %L1\n\tcpy %H0, %H1  // mov64: =h, r"
;;)

;;;; Use this one
;;(define_expand "mov<mode>"
;;  [(set (match_operand:MOV64 0 "nonimmediate_operand" "")
;;    (match_operand:MOV64 1 "general_operand" ""))]
;;  ""
;;{
;;  snowhousecpu_emit_mov (operands[0], operands[1], <MODE>mode);
;;  DONE;
;;})
;;(define_insn "*mov64"
;;  [(set (match_operand:MOV64 0
;;    "nonimmediate_operand"
;;    "=r,r,r,B,r,W")
;;    (match_operand:MOV64 1
;;    "snowhousecpu_general_mov64_src_operand"
;;    ;;"general_operand"
;;    "r,i,B,r,W,r"))]
;;  "register_operand (operands[0], <MODE>mode)
;;  || register_operand (operands[1], <MODE>mode)"
;;{
;;  //return snowhousecpu_output_mov64 (operands[0], operands[1]);
;;  switch (which_alternative)
;;  {
;;    case 0: // =r, r
;;      return "cpy %L0, %L1\n\tcpy %H0, %H1    // *mov64: =r, r";
;;    case 1: // =r, i
;;      //return "cpy %L0, #%1\n\tcpy %H0, #0    // *mov64: =r, i";
;;      //return "cpy %L0, #(%1) >> 32\n\tcpy %H0, #(%1) & 0xffffffff"
;;      //  "    // *mov64: =r, i";
;;    {
;;      //if (!CONST_DOUBLE_P (operands[1]))
;;      {
;;        return "cpy %L0, #(%1) & 0xffffffff\n\tcpy %H0, #(%1) >> 32"
;;          "    // *mov64: =r, i";
;;      }
;;      //else // if (CONST_DOUBLE_P (operands[1]))
;;      //{
;;      //  return "cpy64 %H0, %L0, #%1    // *mov64: =r, const_double";
;;      //}
;;    }
;;    //case 2: // =r, Q
;;    //  return "cpy %L0, #(%1) >> 32\n\tcpy %H0, #(%1) & 0xffffffff"
;;    //    "    // *mov64: =r, Q";
;;    //case 2: // =r, F
;;    //  //return "cpy %L0, #(%1) >> 32\n\tcpy %H0, #(%1) & 0xffffffff"
;;    //  //  "    // *mov64: =r, E";
;;    //  return "cpy64 %H0, %L0, #%1    // *mov64: =r, F";
;;    case 2: // =r, B
;;      if (snowhousecpu_32_bit_offset_address_reg_plus_reg_p (operands[1]))
;;      {
;;        return "ldr %L0, [%L1, #4]\n\tldr %H0, [%L1, #0]"
;;          "    // *mov64: =r, B";
;;      }
;;      else
;;      {
;;        return "ldr %L0, [%L1+4]\n\tldr %H0, [%L1+0]    // *mov64: =r, B";
;;      }
;;    case 3: // =B, r
;;      if (snowhousecpu_32_bit_offset_address_reg_plus_reg_p (operands[1]))
;;      {
;;        return "str %L1, [%L0, #4]\n\tstr %H1, [%L0, #0]"
;;          "    // *mov64: =B, r";
;;      }
;;      else
;;      {
;;        return "str %L1, [%L0+4]\n\tstr %H1, [%L0+0]    // *mov64: =B, r";
;;      }
;;    case 4: // =r, W
;;      return "ldr %L0, [%L1, #4]\n\tldr %H0, [%L1, #0]"
;;        "    // *mov64: =r, W";
;;    case 5: // =W, r
;;      return "str %L1, [%L0, #4]\n\tstr %H1, [%L0, #0]"
;;        "    // *mov64: =W, r";
;;    default:
;;      gcc_unreachable ();
;;  }
;;})
;; --------
(define_expand "mov<mode>"
  [(set (match_operand:MOV16 0 "nonimmediate_operand" "")
    (match_operand:MOV16 1 "snowhousecpu_general_mov_src_operand" ""))]
  ""
{
  //// If this is a store, force the value into a register.
  //if (MEM_P (operands[0]))
  //{
  //  operands[1] = force_reg (HImode, operands[1]);
  //}
  snowhousecpu_emit_mov (operands[0], operands[1], <MODE>mode);
  DONE;
})

(define_insn "*mov16"
  [(set (match_operand:MOV16 0 "nonimmediate_operand"
    "=r,r,h,r,r,B"
    ;;"=r,r,r,W"
    )
    (match_operand:MOV16 1 "snowhousecpu_general_mov_src_operand"
     "r,h,r,i,B,r"
     ;;"r,i,W,r"
     ))]

  "register_operand (operands[0], <MODE>mode)
  || register_operand (operands[1], <MODE>mode)"
  ;;""
  "@
  cpy %0, %1    // *mov16: =r, r
  cpy %0, %1    // *mov16: =r, h
  cpy %0, %1    // *mov16: =h, r
  cpy %0, %1    // *mov16: =r, i
  lduh %0, %1    // *mov16: =r, B
  sth %1, %0    // *mov16: =B, r"
)
;; --------
(define_expand "mov<mode>"
  [(set (match_operand:MOV8 0 "nonimmediate_operand" "")
    (match_operand:MOV8 1 "snowhousecpu_general_mov_src_operand" ""))]
  ""
{
  //// If this is a store, force the value into a register.
  //if (MEM_P (operands[0]))
  //{
  //  operands[1] = force_reg (HImode, operands[1]);
  //}
  snowhousecpu_emit_mov (operands[0], operands[1], <MODE>mode);
  DONE;
})

;; Use this
(define_insn "*mov8"
  [(set (match_operand:MOV8 0 "nonimmediate_operand"
    "=r,r,h,r,r,B"
    )
    (match_operand:MOV8 1 "snowhousecpu_general_mov_src_operand"
      "r,h,r,i,B,r"
      ))]

  "register_operand (operands[0], <MODE>mode)
  || register_operand (operands[1], <MODE>mode)"
  ;;""
  "@
  cpy %0, %1    // *mov8: =r, r
  cpy %0, %1    // *mov8: =r, h
  cpy %0, %1    // *mov8: =h, r
  cpy %0, %1    // *mov8: =r, i
  ldub %0, %1    // *mov8: =r, B
  stb %1, %0    // *mov8: =B, r"
)
;; --------
(define_insn "zero_extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (zero_extend:SI (match_operand:HI 1 "nonimmediate_operand" "B")))]
  ""
  "lduh %0, %1    // zero_extendhisi2"
)

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (sign_extend:SI (match_operand:HI 1 "nonimmediate_operand" "B")))]
  ""
  "ldsh %0, %1    // sign_extendhisi2"
)


(define_insn "zero_extendqisi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (zero_extend:SI (match_operand:QI 1 "nonimmediate_operand" "B")))]
  ""
  "ldub %0, %1    // zero_extendqisi2"
)

(define_insn "sign_extendqisi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (sign_extend:SI (match_operand:QI 1 "nonimmediate_operand" "B")))]
  ""
  "ldsb %0, %1    // sign_extendqisi2"
)

;;(define_insn "cmpdi"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 0 "register_operand" "r")
;;      (match_operand:DI 1 "nonmemory_operand" "r")))]
;;  ""
;;  "cmpd %0, %1    // cmpdi: r r")

;;(define_insn "cmpsi"
;;  [(set (reg:CC REG_FAKE_FLAGS)
;;    (compare:CC (match_operand:SI 0 "register_operand" "r")
;;      (match_operand:SI 1 "nonmemory_operand" "r")))]
;;  ""
;;  "@
;;  // cmpsi: r, r"
;;)

;;(define_insn "cmphi"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:HI 0 "register_operand" "r")
;;      (match_operand:HI 1 "register_operand" "r")))]
;;  ""
;;  "cmph %0, %1    // cmphi: r, r")
;;(define_insn "cmpqi"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:QI 0 "register_operand" "r")
;;      (match_operand:QI 1 "register_operand" "r")))]
;;  ""
;;  "cmpb %0, %1    // cmpqi: r, r")

;;(define_expand "cbranchdi4"
;;  [(set (match_operand:DI 1 "register_operand" "=r")
;;    (minus:DI
;;      (match_operand:DI 2 "register_operand" "%0")
;;      (match_operand:DI 3 "register_operand" "r")))
;;  (set (reg:CC REG_FLAGS)
;;    (compare:CC (match_dup 1) (match_dup 3)))
;;  (set (pc)
;;    (if_then_else (match_operator 0 "comparison_operator"
;;      [(reg:CC REG_FLAGS) (const_int 0)])
;;      (label_ref (match_operand 4 "" ""))
;;      (pc)))]
;;  ""
;;  ""
;;)

;;(define_expand "cbranchdi4"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 1 "register_operand" "r")
;;      (match_operand:DI 2 "register_operand" "r")))
;;    (set (pc)
;;      (if_then_else (match_operator 0 "comparison_operator"
;;        [(reg:CC REG_FLAGS) (const_int 0)])
;;        (label_ref (match_operand 3 "" ""))
;;        (pc)))]
;;  ""
;;  ""
;;)

;;(define_expand "cbranchdi4"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 1 "register_operand" "r")
;;      (match_operand:DI 2 "register_operand" "r")))
;;    (set (pc)
;;      (if_then_else (match_operator 0 "comparison_operator"
;;        [(reg:CC REG_FLAGS) (const_int 0)])
;;        (label_ref (match_operand 3 "" ""))
;;        (pc)))]
;;  ""
;;  ""
;;)
;;--------
;; Use this one
;;(define_expand "cbranchsi4"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:SI 1 "register_operand" "r,r")
;;      (match_operand:SI 2 "nonmemory_operand" "r,i")))
;;    (set (pc)
;;      (if_then_else (match_operator 0 "comparison_operator"
;;        [(reg:CC REG_FLAGS) (const_int 0)])
;;        (label_ref (match_operand 3 "" ""))
;;        (pc)))]
;;  ""
;;{
;;  // Force the things being compared into registers (or an immediate)
;;  //if ((GET_CODE (operands[1]) != REG)
;;  //  || (REGNO_REG_CLASS (REGNO (operands[1])) != GENERAL_REGS))
;;  //{
;;  //  //operands[1] = force_reg (SImode, operands[1]);
;;  //}
;;  ////if (GET_CODE (operands[2]) != REG)
;;  ////{
;;  ////  operands[2] = force_reg (SImode, operands[2]);
;;  ////}
;;  //if ((GET_CODE (operands[2]) != REG)
;;  //  || (GET_CODE (operands[2]) != CONST_INT))
;;  //{
;;  //  operands[2] = force_reg (SImode, operands[2]);
;;  //}
;;})
;;--------
;;(define_expand "cbranchhi4"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:HI 1 "register_operand" "r")
;;      (match_operand:HI 2 "register_operand" "r")))
;;    (set (pc)
;;      (if_then_else (match_operator 0 "comparison_operator"
;;        [(reg:CC REG_FLAGS) (const_int 0)])
;;        (label_ref (match_operand 3 "" ""))
;;        (pc)))]
;;  ""
;;  ""
;;)
;;(define_expand "cbranchqi4"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:QI 1 "register_operand" "r")
;;      (match_operand:QI 2 "register_operand" "r")))
;;    (set (pc)
;;      (if_then_else (match_operator 0 "comparison_operator"
;;        [(reg:CC REG_FLAGS) (const_int 0)])
;;        (label_ref (match_operand 3 "" ""))
;;        (pc)))]
;;  ""
;;  ""
;;)
;;
(define_expand "cbranchsi4"
  [(set (pc)
    (if_then_else
      (match_operator 0 "comparison_operator"
        [(match_operand:SI 1 "general_operand")
        (match_operand:SI 2 "general_operand")])
      (label_ref (match_operand 3 ""))
      (pc)))]
  ""
{
  const enum rtx_code code = GET_CODE (operands[0]);
  //operands[1] = force_reg (operands[1]);
  //operands[2] = force_reg (operands[2]);
  rtx op1 = operands[1];
  rtx op2 = operands[2];
  rtx label3 = operands[3];
  rtx j;
  rtx my_cond;
  if (GET_CODE (op1) != REG) {
    op1 = force_reg (SImode, op1);
  }
  if (GET_CODE (op2) != REG) {
    op2 = force_reg (SImode, op2);
  }
  if (
    code == EQ || code == NE
    || code == GEU || code == LTU || code == GTU || code == LEU
    || code == GE || code == LT || code == GT || code == LE
  )
  {
    my_cond = gen_rtx_fmt_ee (code, VOIDmode, op1, op2);
  }
  else
  {
    //fprintf (
    //  stderr,
    //  "test\n"
    //);
    gcc_unreachable ();
  }
  //rtx label3_ref = gen_rtx_LABEL_REF (Pmode, label3);
  //rtx check = gen_rtx_IF_THEN_ELSE (VOIDmode, my_cond, label3, pc_rtx);
  //j = emit_jump_insn (gen_rtx_SET (pc_rtx, check));
  //emit_conditional_branch_insn ();
  //JUMP_LABEL (j) = label3;
  //LABEL_NUSES (label3)++;
  emit_jump_insn (gen_condjump (my_cond, label3));
  DONE;
})
(define_expand "condjump"
  [(set (pc)
    (if_then_else (match_operand 0)
              (label_ref (match_operand 1))
              (pc)))])
(define_insn "*branch"
  [(set (pc)
    (if_then_else
      (match_operator 1 "ordered_comparison_operator"
        [(match_operand:SI 2 "register_operand" "r")
        (match_operand:SI 3 "register_operand" "r")])
      (label_ref (match_operand 0 ""))
      (pc)))]
  ""
  "b%C1 %2, %3, %l0"
)
;;(define_insn "beq"
;;  [(set (pc)
;;    (if_then_else
;;      (eq
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "beq %0, %1, %2")
;;(define_insn "bne"
;;  [(set (pc)
;;    (if_then_else
;;      (ne
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "bne %0, %1, %2")
;;(define_insn "bgeu"
;;  [(set (pc)
;;    (if_then_else
;;      (geu
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "bgeu %0, %1, %2")
;;(define_insn "bltu"
;;  [(set (pc)
;;    (if_then_else
;;      (ltu
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "bltu %0, %1, %2")
;;(define_insn "bgtu"
;;  [(set (pc)
;;    (if_then_else
;;      (gtu
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "bgtu %0, %1, %2")
;;(define_insn "bleu"
;;  [(set (pc)
;;    (if_then_else
;;      (leu
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "bleu %0, %1, %2")
;;(define_insn "bge"
;;  [(set (pc)
;;    (if_then_else
;;      (ge
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "bge %0, %1, %2")
;;(define_insn "blt"
;;  [(set (pc)
;;    (if_then_else
;;      (lt
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "blt %0, %1, %2")
;;(define_insn "bgt"
;;  [(set (pc)
;;    (if_then_else
;;      (gt
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "bgt %0, %1, %2")
;;(define_insn "ble"
;;  [(set (pc)
;;    (if_then_else
;;      (le
;;        (match_operand:SI 0 "register_operand" "r")
;;        (match_operand:SI 1 "register_operand" "r"))
;;      (label_ref (match_operand 2 ""))
;;      (pc)))]
;;  ""
;;  "ble %0, %1, %2")

;;(define_insn "mybranch<cond:code>"
;;  [(set (pc)
;;    (if_then_else (cond (reg:CC REG_FAKE_FLAGS) (const_int 0))
;;      (label_ref (match_operand 0 "" ""))
;;      (pc)))]
;;  ""
;;  "b<CC> %l0")
;;--------
;;(define_expand "cbranchsi4"
;;  [(set (pc)
;;    (if_then_else (match_operator 0 "comparison_operator"
;;      [(match_operand:SI 1 "register_operand")
;;        (match_operand:SI 2 "register_operand")
;;      (label_ref (match_operand 3 ""))
;;      (pc)])))]
;;  ""
;;{
;;  snowhousecpu_expand_conditional_branch (operands);
;;  DONE;
;;})
;;(define_insn "cbranchsi4"
;;  [(use (match_operator 0 "comparison_operator"
;;         [(match_operand:SI 1 "register_operand" "")
;;          (match_operand:SI 2 "register_operand" "")]))
;;   (use (match_operand 3 ""))]
;;  ""
;;{
;;  return snowhousecpu_expand_conditional_branch (operands);
;;  //snowhousecpu_expand_conditional_branch (operands);
;;  //DONE;
;;})
;;--------
;;(define_insn "cbranchsi4"
;;  [(set (pc)
;;    (if_then_else (match_operator 0 "comparison_operator"
;;      [(match_operand:SI 1 "register_operand")
;;        (match_operand:SI 2 "register_operand")
;;      (label_ref (match_operand 3 ""))
;;      (pc))))]
;;  ""
;;)

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "register_operand" "r"))]
  ""
  "jmp %0")

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0)))]
  ""
  "bl r0, %l0"
  )


;;(define_expand "call"
;;  [(parallel [(call (mem:SI (match_operand 0 "nonmemory_operand" ""))
;;      (match_operand 1 ""))
;;    ;;(match_operand 2 "")
;;    (clobber (reg:SI REG_LR))])]
;;  ""
;;{
;;  gcc_assert (MEM_P (operands[0]));
;;})
(define_expand "call"
  [(parallel [(call (match_operand:SI 0 "memory_operand" "")
      (match_operand 1 ""))
    ;;(match_operand 2 "")
    ;;(use (match_operand 2 ""))
    (clobber (reg:SI REG_LR))])]
  ""
{
  gcc_assert (MEM_P (operands[0]));
})
(define_insn "*call"
  [(call (mem:SI (match_operand:SI 0 "nonmemory_operand" "i,r"))
      (match_operand 1 ""))
    ;;(match_operand 2 "")
    ;;(use (match_operand 2 ""))
    (clobber (reg:SI REG_LR))]
  ""
  "@
  bl %0    // *call: i
  jl lr, %0    // *call: r")


(define_expand "call_value"
  [(parallel [(set (match_operand 0 "register_operand" "")
    (call (match_operand:SI 1 "memory_operand" "")
      (match_operand 2 "")))
    ;;(match_operand 3 "")
    ;;(use (match_operand 3 ""))
    (clobber (reg:SI REG_LR))])]
  ""
{
  gcc_assert (MEM_P (operands[1]));
  //snowhousecpu_dbg_call_value (operands[0], operands[1], operands[2]);
  //DONE;
})
;;
;;(define_expand "call_value"
;;  [(parallel [(set (match_operand 0 "" "")
;;    (call (mem:HI (match_operand 1 "nonmemory_operand" ""))
;;      (match_operand 2 "")))
;;    (clobber (reg:SI REG_LR))])]
;;  ""
;;{
;;  gcc_assert (MEM_P (operands[1]));
;;  //if (MEM_P (XEXP (operands[1], 0)))
;;  //{
;;  //  if (can_create_pseudo_p ())
;;  //  {
;;  //    rtx tmp = force_reg (GET_MODE (XEXP (operands[1], 0)),
;;  //      XEXP (operands[1], 0));
;;  //    operands[1] = gen_rtx_MEM (GET_MODE (operands[1])
;;  //  }
;;  //}
;;})

;;(define_insn "*call_value"
;;  [(set (match_operand 0 "register_operand" "=r,r")
;;    (call (mem:HI (match_operand 1 "nonmemory_operand" "i,r"))
;;      (match_operand 2 "")))
;;    ;;(match_operand 3 "")
;;    (clobber (reg:SI REG_LR))]
;;  ""
;;  "@
;;  bl %1    // *call_value: =r, i
;;  jl %1    // *call_value: =r, i")
;;(define_insn "*call_value"
;;  [(set (match_operand 0 "register_operand" "=r,r")
;;    (call (mem:SI (match_operand:SI 1 "address_operand" "i,r"))
;;      (match_operand 2 "")))
;;    ;;(match_operand 3 "")
;;    ;;(use (match_operand 3 ""))
;;    (clobber (reg:SI REG_LR))]
;;  ""
;;  "@
;;  bl(\"%1\"),    // *call_value: =r, i
;;  jl(%1),    // *call_value: =r, r"
;;)
(define_insn "*call_value"
  [(set (match_operand 0 "register_operand" "=r,r")
    (call (mem:SI (match_operand:SI 1 "address_operand" "i,r"))
      (match_operand 2 "")))
    ;;(match_operand 3 "")
    ;;(use (match_operand 3 ""))
    (clobber (reg:SI REG_LR))]
  ""
  "@
  bl %1    // *call_value: =r, i
  jl lr, %1    // *call_value: =r, r"
)

;;(define_insn "*call_value_indirect"
;;  [(set (match_operand 0 "register_operand" "=r")
;;    (call (mem:SI (match_operand:SI 1 "register_operand" "r"))
;;      (match_operand 2 "" "")))
;;      (clobber (reg:SI REG_LR))]
;;  ""
;;  "jl %1    // *call_value_indirect: =r, r")
;;
;;;; borrowed from RISC-V
;;(define_insn "stack_tie<mode>"
;;  [(set (mem:BLK (scratch))
;;    (unspec:BLK [(match_operand:X 0 "register_operand" "r")
;;      (match_operand:X 1 "register_operand" "r")]
;;      UNSPEC_TIE))]
;;  ""
;;  ""
;;  [(set_attr "length" "0")]
;;)

;; --------------
;; Prologue
;; --------------
(define_expand "prologue"
  [(clobber (const_int 1))]
  ""
  "
{
  snowhousecpu_expand_prologue ();
  DONE;
}")

;; --------------
;; Epilogue
;; --------------
(define_expand "epilogue"
  [(return)]
  ""
  "
{
  snowhousecpu_expand_epilogue ();
  DONE;
}")

(define_insn
  "returner"
  [(return)]
  "reload_completed"
  "jmp lr")
;;(define_insn
;;  "return"
;;  [(return)]
;;  "reload_completed"
;;  "jmp lr")
;;(define_insn
;;  "simple_return"
;;  [(return)]
;;  "reload_completed"
;;  "jmp lr")
