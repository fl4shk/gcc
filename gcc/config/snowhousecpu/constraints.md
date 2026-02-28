;; Constraint definitions for SnowHouseCpu
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

;;(define_register_constraint "r" "GENERAL_REGS"
;;  "A general purpose register")
;;(define_register_constraint "f" "CC_REGS" "flags")
;;(define_register_constraint "z" "FULL_PRODUCT_RESULT_REGS" "r0, r1")
;;(define_register_constraint "x" "FULL_PRODUCT_HIGH_PART_REGS" "r0")
;;(define_register_constraint "y" "FULL_PRODUCT_LOW_PART_REGS" "r1")
(define_register_constraint "d" "PC_REGS")
(define_register_constraint "t" "SP_REGS")
(define_register_constraint "q" "FP_REGS")
(define_register_constraint "R" "GENERAL_REGS")
(define_register_constraint "w" "HI_REGS")
;;(define_register_constraint "W" "GENERAL_OR_HI_REGS")
(define_register_constraint "h" "DIVW_OUTP_REGS")

(define_memory_constraint "B"
    "An offset adddress (register + immediate)"
    (and (match_code "mem")
        (match_test "snowhousecpu_offset_address_reg_plus_imm_p (op)")))
;;(define_memory_constraint "B"
;;  "An offset address (32-bit: (register + immediate) | (register + register) | (register + register + immediate))."
;;  (and (match_code "mem")
;;    (match_test "snowhousecpu_32_bit_offset_address_p (op)")))
;;(define_memory_constraint "W"
;;  "A register indirect memory operand."
;;  (and (match_code "mem")
;;    (match_test "snowhousecpu_non_offset_address_p (op)")))
;;(define_memory_constraint "C"
;;  "An offset address (8-bit/16-bit: register + register)."
;;  (and (match_code "mem")
;;    (match_test "snowhousecpu_8_or_16_bit_offset_address_p (op)")))
