//extern void snowhousecpu_expand_compare_and_swap (rtx operands[]);
extern int snowhousecpu_initial_elimination_offset (int from, int to);
//extern bool snowhousecpu_32_bit_offset_address_p (rtx x);

//extern bool snowhousecpu_32_bit_offset_address_reg_plus_reg_p (rtx x);
extern bool snowhousecpu_offset_address_reg_plus_imm_p (rtx x);
//extern bool snowhousecpu_32_bit_offset_address_reg_plus_reg_plus_imm_p (rtx x);
//extern bool snowhousecpu_8_or_16_bit_offset_address_p (rtx x);
extern bool snowhousecpu_non_offset_address_p (rtx x);
//extern bool snowhousecpu_non_offset_address_p (rtx x);

extern bool snowhousecpu_absolute (rtx x);
extern bool snowhousecpu_absolute_ex (rtx x);
//extern bool snowhousecpu_absolute_plus_ci (rtx x);

//extern bool snowhousecpu_reg_cpyspec (rtx x);
//extern bool snowhousecpu_reg_class_cpyspec (reg_class x);
extern bool snowhousecpu_reg_general (rtx x);
extern bool snowhousecpu_reg_class_general (reg_class x);


extern bool snowhousecpu_regno_actually_callee_saved (int regno);
//extern void snowhousecpu_compute_frame ();

extern void snowhousecpu_expand_prologue ();
extern void snowhousecpu_expand_epilogue ();

//extern void snowhousecpu_expand_mov32 (rtx& dst, rtx& src, machine_mode mode);
//extern bool snowhousecpu_expand_mov64 (rtx& dst, rtx& src, machine_mode mode);
//extern bool snowhousecpu_legitimize_movdi (rtx dst, rtx src);
//extern const char* snowhousecpu_output_movqi (rtx dst, rtx src);
//extern const char* snowhousecpu_output_movhi (rtx dst, rtx src);
//extern void snowhousecpu_emit_mov8 (rtx dst, rtx src, machine_mode mode);
//extern void snowhousecpu_emit_mov16 (rtx dst, rtx src, machine_mode mode);
//extern void snowhousecpu_emit_mov32 (rtx dst, rtx src, machine_mode mode);
extern void snowhousecpu_emit_mov (rtx dst, rtx src, machine_mode mode);
//extern const char* snowhousecpu_output_movdi (rtx dst, rtx src);
//extern void snowhousecpu_dbg_call_value (rtx retop, rtx target, rtx op2);
extern void snowhousecpu_gen_cmpdi_reg (rtx x, rtx y);
extern rtx snowhousecpu_get_return_address (int count);
//extern const char* snowhousecpu_expand_conditional_branch (rtx *operands);

//extern int snowhousecpu_stack_pointer_offset ();
//extern int snowhousecpu_incoming_frame_sp_offset ();
extern bool snowhousecpu_push_argument (unsigned int npush);
//extern long long snowhousecpu_stack_pointer_offset ();
