#include "./prtty.hpp"
#include <iostream>

using namespace std;

int main() {
	prtty::term term = prtty::get();
	cout << "detected terminal: " << term.id << endl;
	cout << "common names:" << endl;
	for (auto name : term.names) {
		cout << "\t- " << name << endl;
	}

#define TEST_NARGS(name) { size_t nargs = prtty::impl::Sequence::parse(term.name).nargs; if (nargs > 0) cout << nargs << "\t" #name << endl; }

	TEST_NARGS(back_tab);
	TEST_NARGS(bell);
	TEST_NARGS(carriage_return);
	TEST_NARGS(change_scroll_region);
	TEST_NARGS(clear_all_tabs);
	TEST_NARGS(clear_screen);
	TEST_NARGS(clr_eol);
	TEST_NARGS(clr_eos);
	TEST_NARGS(column_address);
	TEST_NARGS(command_character);
	TEST_NARGS(cursor_address);
	TEST_NARGS(cursor_down);
	TEST_NARGS(cursor_home);
	TEST_NARGS(cursor_invisible);
	TEST_NARGS(cursor_left);
	TEST_NARGS(cursor_mem_address);
	TEST_NARGS(cursor_normal);
	TEST_NARGS(cursor_right);
	TEST_NARGS(cursor_to_ll);
	TEST_NARGS(cursor_up);
	TEST_NARGS(cursor_visible);
	TEST_NARGS(delete_character);
	TEST_NARGS(delete_line);
	TEST_NARGS(dis_status_line);
	TEST_NARGS(down_half_line);
	TEST_NARGS(enter_alt_charset_mode);
	TEST_NARGS(enter_blink_mode);
	TEST_NARGS(enter_bold_mode);
	TEST_NARGS(enter_ca_mode);
	TEST_NARGS(enter_delete_mode);
	TEST_NARGS(enter_dim_mode);
	TEST_NARGS(enter_insert_mode);
	TEST_NARGS(enter_secure_mode);
	TEST_NARGS(enter_protected_mode);
	TEST_NARGS(enter_reverse_mode);
	TEST_NARGS(enter_standout_mode);
	TEST_NARGS(enter_underline_mode);
	TEST_NARGS(erase_chars);
	TEST_NARGS(exit_alt_charset_mode);
	TEST_NARGS(exit_attribute_mode);
	TEST_NARGS(exit_ca_mode);
	TEST_NARGS(exit_delete_mode);
	TEST_NARGS(exit_insert_mode);
	TEST_NARGS(exit_standout_mode);
	TEST_NARGS(exit_underline_mode);
	TEST_NARGS(flash_screen);
	TEST_NARGS(form_feed);
	TEST_NARGS(from_status_line);
	TEST_NARGS(init_1string);
	TEST_NARGS(init_2string);
	TEST_NARGS(init_3string);
	TEST_NARGS(init_file);
	TEST_NARGS(insert_character);
	TEST_NARGS(insert_line);
	TEST_NARGS(insert_padding);
	TEST_NARGS(key_backspace);
	TEST_NARGS(key_catab);
	TEST_NARGS(key_clear);
	TEST_NARGS(key_ctab);
	TEST_NARGS(key_dc);
	TEST_NARGS(key_dl);
	TEST_NARGS(key_down);
	TEST_NARGS(key_eic);
	TEST_NARGS(key_eol);
	TEST_NARGS(key_eos);
	TEST_NARGS(key_f0);
	TEST_NARGS(key_f1);
	TEST_NARGS(key_f10);
	TEST_NARGS(key_f2);
	TEST_NARGS(key_f3);
	TEST_NARGS(key_f4);
	TEST_NARGS(key_f5);
	TEST_NARGS(key_f6);
	TEST_NARGS(key_f7);
	TEST_NARGS(key_f8);
	TEST_NARGS(key_f9);
	TEST_NARGS(key_home);
	TEST_NARGS(key_ic);
	TEST_NARGS(key_il);
	TEST_NARGS(key_left);
	TEST_NARGS(key_ll);
	TEST_NARGS(key_npage);
	TEST_NARGS(key_ppage);
	TEST_NARGS(key_right);
	TEST_NARGS(key_sf);
	TEST_NARGS(key_sr);
	TEST_NARGS(key_stab);
	TEST_NARGS(key_up);
	TEST_NARGS(keypad_local);
	TEST_NARGS(keypad_xmit);
	TEST_NARGS(lab_f0);
	TEST_NARGS(lab_f1);
	TEST_NARGS(lab_f10);
	TEST_NARGS(lab_f2);
	TEST_NARGS(lab_f3);
	TEST_NARGS(lab_f4);
	TEST_NARGS(lab_f5);
	TEST_NARGS(lab_f6);
	TEST_NARGS(lab_f7);
	TEST_NARGS(lab_f8);
	TEST_NARGS(lab_f9);
	TEST_NARGS(meta_off);
	TEST_NARGS(meta_on);
	TEST_NARGS(newline);
	TEST_NARGS(pad_char);
	TEST_NARGS(parm_dch);
	TEST_NARGS(parm_delete_line);
	TEST_NARGS(parm_down_cursor);
	TEST_NARGS(parm_ich);
	TEST_NARGS(parm_index);
	TEST_NARGS(parm_insert_line);
	TEST_NARGS(parm_left_cursor);
	TEST_NARGS(parm_right_cursor);
	TEST_NARGS(parm_rindex);
	TEST_NARGS(parm_up_cursor);
	TEST_NARGS(pkey_key);
	TEST_NARGS(pkey_local);
	TEST_NARGS(pkey_xmit);
	TEST_NARGS(print_screen);
	TEST_NARGS(prtr_off);
	TEST_NARGS(prtr_on);
	TEST_NARGS(repeat_char);
	TEST_NARGS(reset_1string);
	TEST_NARGS(reset_2string);
	TEST_NARGS(reset_3string);
	TEST_NARGS(reset_file);
	TEST_NARGS(restore_cursor);
	TEST_NARGS(row_address);
	TEST_NARGS(save_cursor);
	TEST_NARGS(scroll_forward);
	TEST_NARGS(scroll_reverse);
	TEST_NARGS(set_attributes);
	TEST_NARGS(set_tab);
	TEST_NARGS(set_window);
	TEST_NARGS(tab);
	TEST_NARGS(to_status_line);
	TEST_NARGS(underline_char);
	TEST_NARGS(up_half_line);
	TEST_NARGS(init_prog);
	TEST_NARGS(key_a1);
	TEST_NARGS(key_a3);
	TEST_NARGS(key_b2);
	TEST_NARGS(key_c1);
	TEST_NARGS(key_c3);
	TEST_NARGS(prtr_non);
	TEST_NARGS(char_padding);
	TEST_NARGS(acs_chars);
	TEST_NARGS(plab_norm);
	TEST_NARGS(key_btab);
	TEST_NARGS(enter_xon_mode);
	TEST_NARGS(exit_xon_mode);
	TEST_NARGS(enter_am_mode);
	TEST_NARGS(exit_am_mode);
	TEST_NARGS(xon_character);
	TEST_NARGS(xoff_character);
	TEST_NARGS(ena_acs);
	TEST_NARGS(label_on);
	TEST_NARGS(label_off);
	TEST_NARGS(key_beg);
	TEST_NARGS(key_cancel);
	TEST_NARGS(key_close);
	TEST_NARGS(key_command);
	TEST_NARGS(key_copy);
	TEST_NARGS(key_create);
	TEST_NARGS(key_end);
	TEST_NARGS(key_enter);
	TEST_NARGS(key_exit);
	TEST_NARGS(key_find);
	TEST_NARGS(key_help);
	TEST_NARGS(key_mark);
	TEST_NARGS(key_message);
	TEST_NARGS(key_move);
	TEST_NARGS(key_next);
	TEST_NARGS(key_open);
	TEST_NARGS(key_options);
	TEST_NARGS(key_previous);
	TEST_NARGS(key_print);
	TEST_NARGS(key_redo);
	TEST_NARGS(key_reference);
	TEST_NARGS(key_refresh);
	TEST_NARGS(key_replace);
	TEST_NARGS(key_restart);
	TEST_NARGS(key_resume);
	TEST_NARGS(key_save);
	TEST_NARGS(key_suspend);
	TEST_NARGS(key_undo);
	TEST_NARGS(key_sbeg);
	TEST_NARGS(key_scancel);
	TEST_NARGS(key_scommand);
	TEST_NARGS(key_scopy);
	TEST_NARGS(key_screate);
	TEST_NARGS(key_sdc);
	TEST_NARGS(key_sdl);
	TEST_NARGS(key_select);
	TEST_NARGS(key_send);
	TEST_NARGS(key_seol);
	TEST_NARGS(key_sexit);
	TEST_NARGS(key_sfind);
	TEST_NARGS(key_shelp);
	TEST_NARGS(key_shome);
	TEST_NARGS(key_sic);
	TEST_NARGS(key_sleft);
	TEST_NARGS(key_smessage);
	TEST_NARGS(key_smove);
	TEST_NARGS(key_snext);
	TEST_NARGS(key_soptions);
	TEST_NARGS(key_sprevious);
	TEST_NARGS(key_sprint);
	TEST_NARGS(key_sredo);
	TEST_NARGS(key_sreplace);
	TEST_NARGS(key_sright);
	TEST_NARGS(key_srsume);
	TEST_NARGS(key_ssave);
	TEST_NARGS(key_ssuspend);
	TEST_NARGS(key_sundo);
	TEST_NARGS(req_for_input);
	TEST_NARGS(key_f11);
	TEST_NARGS(key_f12);
	TEST_NARGS(key_f13);
	TEST_NARGS(key_f14);
	TEST_NARGS(key_f15);
	TEST_NARGS(key_f16);
	TEST_NARGS(key_f17);
	TEST_NARGS(key_f18);
	TEST_NARGS(key_f19);
	TEST_NARGS(key_f20);
	TEST_NARGS(key_f21);
	TEST_NARGS(key_f22);
	TEST_NARGS(key_f23);
	TEST_NARGS(key_f24);
	TEST_NARGS(key_f25);
	TEST_NARGS(key_f26);
	TEST_NARGS(key_f27);
	TEST_NARGS(key_f28);
	TEST_NARGS(key_f29);
	TEST_NARGS(key_f30);
	TEST_NARGS(key_f31);
	TEST_NARGS(key_f32);
	TEST_NARGS(key_f33);
	TEST_NARGS(key_f34);
	TEST_NARGS(key_f35);
	TEST_NARGS(key_f36);
	TEST_NARGS(key_f37);
	TEST_NARGS(key_f38);
	TEST_NARGS(key_f39);
	TEST_NARGS(key_f40);
	TEST_NARGS(key_f41);
	TEST_NARGS(key_f42);
	TEST_NARGS(key_f43);
	TEST_NARGS(key_f44);
	TEST_NARGS(key_f45);
	TEST_NARGS(key_f46);
	TEST_NARGS(key_f47);
	TEST_NARGS(key_f48);
	TEST_NARGS(key_f49);
	TEST_NARGS(key_f50);
	TEST_NARGS(key_f51);
	TEST_NARGS(key_f52);
	TEST_NARGS(key_f53);
	TEST_NARGS(key_f54);
	TEST_NARGS(key_f55);
	TEST_NARGS(key_f56);
	TEST_NARGS(key_f57);
	TEST_NARGS(key_f58);
	TEST_NARGS(key_f59);
	TEST_NARGS(key_f60);
	TEST_NARGS(key_f61);
	TEST_NARGS(key_f62);
	TEST_NARGS(key_f63);
	TEST_NARGS(clr_bol);
	TEST_NARGS(clear_margins);
	TEST_NARGS(set_left_margin);
	TEST_NARGS(set_right_margin);
	TEST_NARGS(label_format);
	TEST_NARGS(set_clock);
	TEST_NARGS(display_clock);
	TEST_NARGS(remove_clock);
	TEST_NARGS(create_window);
	TEST_NARGS(goto_window);
	TEST_NARGS(hangup);
	TEST_NARGS(dial_phone);
	TEST_NARGS(quick_dial);
	TEST_NARGS(tone);
	TEST_NARGS(pulse);
	TEST_NARGS(flash_hook);
	TEST_NARGS(fixed_pause);
	TEST_NARGS(wait_tone);
	TEST_NARGS(user0);
	TEST_NARGS(user1);
	TEST_NARGS(user2);
	TEST_NARGS(user3);
	TEST_NARGS(user4);
	TEST_NARGS(user5);
	TEST_NARGS(user6);
	TEST_NARGS(user7);
	TEST_NARGS(user8);
	TEST_NARGS(user9);
	TEST_NARGS(orig_pair);
	TEST_NARGS(orig_colors);
	TEST_NARGS(initialize_color);
	TEST_NARGS(initialize_pair);
	TEST_NARGS(set_color_pair);
	TEST_NARGS(set_foreground);
	TEST_NARGS(set_background);
	TEST_NARGS(change_char_pitch);
	TEST_NARGS(change_line_pitch);
	TEST_NARGS(change_res_horz);
	TEST_NARGS(change_res_vert);
	TEST_NARGS(define_char);
	TEST_NARGS(enter_doublewide_mode);
	TEST_NARGS(enter_draft_quality);
	TEST_NARGS(enter_italics_mode);
	TEST_NARGS(enter_leftward_mode);
	TEST_NARGS(enter_micro_mode);
	TEST_NARGS(enter_near_letter_quality);
	TEST_NARGS(enter_normal_quality);
	TEST_NARGS(enter_shadow_mode);
	TEST_NARGS(enter_subscript_mode);
	TEST_NARGS(enter_superscript_mode);
	TEST_NARGS(enter_upward_mode);
	TEST_NARGS(exit_doublewide_mode);
	TEST_NARGS(exit_italics_mode);
	TEST_NARGS(exit_leftward_mode);
	TEST_NARGS(exit_micro_mode);
	TEST_NARGS(exit_shadow_mode);
	TEST_NARGS(exit_subscript_mode);
	TEST_NARGS(exit_superscript_mode);
	TEST_NARGS(exit_upward_mode);
	TEST_NARGS(micro_column_address);
	TEST_NARGS(micro_down);
	TEST_NARGS(micro_left);
	TEST_NARGS(micro_right);
	TEST_NARGS(micro_row_address);
	TEST_NARGS(micro_up);
	TEST_NARGS(order_of_pins);
	TEST_NARGS(parm_down_micro);
	TEST_NARGS(parm_left_micro);
	TEST_NARGS(parm_right_micro);
	TEST_NARGS(parm_up_micro);
	TEST_NARGS(select_char_set);
	TEST_NARGS(set_bottom_margin);
	TEST_NARGS(set_bottom_margin_parm);
	TEST_NARGS(set_left_margin_parm);
	TEST_NARGS(set_right_margin_parm);
	TEST_NARGS(set_top_margin);
	TEST_NARGS(set_top_margin_parm);
	TEST_NARGS(start_bit_image);
	TEST_NARGS(start_char_set_def);
	TEST_NARGS(stop_bit_image);
	TEST_NARGS(stop_char_set_def);
	TEST_NARGS(subscript_characters);
	TEST_NARGS(superscript_characters);
	TEST_NARGS(these_cause_cr);
	TEST_NARGS(zero_motion);
	TEST_NARGS(char_set_names);
	TEST_NARGS(key_mouse);
	TEST_NARGS(mouse_info);
	TEST_NARGS(req_mouse_pos);
	TEST_NARGS(get_mouse);
	TEST_NARGS(set_a_foreground);
	TEST_NARGS(set_a_background);
	TEST_NARGS(pkey_plab);
	TEST_NARGS(device_type);
	TEST_NARGS(code_set_init);
	TEST_NARGS(set0_des_seq);
	TEST_NARGS(set1_des_seq);
	TEST_NARGS(set2_des_seq);
	TEST_NARGS(set3_des_seq);
	TEST_NARGS(set_lr_margin);
	TEST_NARGS(set_tb_margin);
	TEST_NARGS(bit_image_repeat);
	TEST_NARGS(bit_image_newline);
	TEST_NARGS(bit_image_carriage_return);
	TEST_NARGS(color_names);
	TEST_NARGS(define_bit_image_region);
	TEST_NARGS(end_bit_image_region);
	TEST_NARGS(set_color_band);
	TEST_NARGS(set_page_length);
	TEST_NARGS(display_pc_char);
	TEST_NARGS(enter_pc_charset_mode);
	TEST_NARGS(exit_pc_charset_mode);
	TEST_NARGS(enter_scancode_mode);
	TEST_NARGS(exit_scancode_mode);
	TEST_NARGS(pc_term_options);
	TEST_NARGS(scancode_escape);
	TEST_NARGS(alt_scancode_esc);
	TEST_NARGS(enter_horizontal_hl_mode);
	TEST_NARGS(enter_left_hl_mode);
	TEST_NARGS(enter_low_hl_mode);
	TEST_NARGS(enter_right_hl_mode);
	TEST_NARGS(enter_top_hl_mode);
	TEST_NARGS(enter_vertical_hl_mode);
	TEST_NARGS(set_a_attributes);
	TEST_NARGS(set_pglen_inch);


	return 0;
}
