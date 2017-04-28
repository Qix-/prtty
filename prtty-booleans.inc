#ifndef PRTTY_DO_FIRST_BOOLEAN
#	define PRTTY_DO_FIRST_BOOLEAN PRTTY_DO_BOOLEAN
#endif
#ifndef PRTTY_DO_LAST_BOOLEAN
#	define PRTTY_DO_LAST_BOOLEAN PRTTY_DO_BOOLEAN
#endif

#ifndef PRTTY_FIRST_BOOLEAN
#	define PRTTY_FIRST_BOOLEAN auto_left_margin
#endif
PRTTY_DO_FIRST_BOOLEAN(auto_left_margin)
PRTTY_DO_BOOLEAN(auto_right_margin)
PRTTY_DO_BOOLEAN(no_esc_ctlc)
PRTTY_DO_BOOLEAN(ceol_standout_glitch)
PRTTY_DO_BOOLEAN(eat_newline_glitch)
PRTTY_DO_BOOLEAN(erase_overstrike)
PRTTY_DO_BOOLEAN(generic_type)
PRTTY_DO_BOOLEAN(hard_copy)
PRTTY_DO_BOOLEAN(has_meta_key)
PRTTY_DO_BOOLEAN(has_status_line)
PRTTY_DO_BOOLEAN(insert_null_glitch)
PRTTY_DO_BOOLEAN(memory_above)
PRTTY_DO_BOOLEAN(memory_below)
PRTTY_DO_BOOLEAN(move_insert_mode)
PRTTY_DO_BOOLEAN(move_standout_mode)
PRTTY_DO_BOOLEAN(over_strike)
PRTTY_DO_BOOLEAN(status_line_esc_ok)
PRTTY_DO_BOOLEAN(dest_tabs_magic_smso)
PRTTY_DO_BOOLEAN(tilde_glitch)
PRTTY_DO_BOOLEAN(transparent_underline)
PRTTY_DO_BOOLEAN(xon_xoff)
PRTTY_DO_BOOLEAN(needs_xon_xoff)
PRTTY_DO_BOOLEAN(prtr_silent)
PRTTY_DO_BOOLEAN(hard_cursor)
PRTTY_DO_BOOLEAN(non_rev_rmcup)
PRTTY_DO_BOOLEAN(no_pad_char)
PRTTY_DO_BOOLEAN(non_dest_scroll_region)
PRTTY_DO_BOOLEAN(can_change)
PRTTY_DO_BOOLEAN(back_color_erase)
PRTTY_DO_BOOLEAN(hue_lightness_saturation)
PRTTY_DO_BOOLEAN(col_addr_glitch)
PRTTY_DO_BOOLEAN(cr_cancels_micro_mode)
PRTTY_DO_BOOLEAN(has_print_wheel)
PRTTY_DO_BOOLEAN(row_addr_glitch)
PRTTY_DO_BOOLEAN(semi_auto_right_margin)
PRTTY_DO_BOOLEAN(cpi_changes_res)
PRTTY_DO_LAST_BOOLEAN(lpi_changes_res)

#undef PRTTY_DO_BOOLEAN
#undef PRTTY_DO_FIRST_BOOLEAN
#undef PRTTY_DO_LAST_BOOLEAN