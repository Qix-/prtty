#ifndef PRTTY_H__
#define PRTTY_H__
#pragma once

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <stack>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace prtty {
	using namespace std;

	class PrttyError : public runtime_error {
	public:
		template <typename... Args>
		PrttyError(Args... args)
				: runtime_error(args...) {
		}
	};

	namespace impl {

		struct Any {
			enum class Type {
				STRING,
				INT,
				CHAR
			};

			Any()
					: type(Type::INT)
					, tint(0) {
			}

			Any(const char *str)
					: type(Type::STRING)
					, tstring(str) {
			}

			Any(int i)
					: type(Type::INT)
					, tint(i) {
			}

			Any(char c)
					: type(Type::CHAR)
					, tchar(c) {
			}

			Type type;

			union {
				const char *tstring;
				int tint;
				char tchar;
			};
		};

		struct Data {
			/*
				we create data structs to avoid re-allocation
				of stacks on each parse. these are hidden from
				the user and stored in `term` objects.
			*/

			Data() {
				this->dparm.reserve(26);
				this->sparm.reserve(26);
				fill(this->dparm.begin(), this->dparm.end(), Any(0));
				fill(this->sparm.begin(), this->sparm.end(), Any(0));
			}

			template <typename... Args>
			void session(Args... args) {
				stack<Any>().swap(this->stk);
				this->dparm.clear();
				this->params = vector<Any>(9, Any());
				this->params.insert(this->params.begin(), {args...});
			}

			stack<Any> stk;
			vector<Any> params;
			vector<Any> dparm;

			vector<Any> sparm;
		};

		struct Operation {
			typedef vector<unique_ptr<Operation>>::const_iterator OpItr;

			virtual ~Operation() = default;
			virtual void operator()(ostream &stream, Data &data, OpItr &citr) = 0;
			virtual bool isControlOp() const {
				return false;
			}
		};

		namespace op {
			struct StringLiteral : public Operation {
				const string literal;

				StringLiteral(string literal) : literal(literal) {}
				virtual ~StringLiteral() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) data;
					stream << this->literal;
				}
			};

			struct IntLiteral : public Operation {
				const int literal;

				IntLiteral(int literal) : literal(literal) {}
				virtual ~IntLiteral() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) data;
					stream << this->literal;
				}
			};

			struct CharLiteral : public Operation {
				const char literal;

				CharLiteral(char literal) : literal(literal) {}
				virtual ~CharLiteral() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) data;
					stream << string(1, this->literal);
				}
			};

			struct PushArg : public Operation {
				const unsigned int arg;

				PushArg(unsigned int arg) : arg(arg) {
					if (arg > 8) {
						throw prtty::PrttyError("arg value must be between 0-8 (inclusive)");
					}
				}
				virtual ~PushArg() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) stream;
					data.stk.push(data.params[this->arg]);
				}
			};

			struct PushStrlen : public Operation {
				virtual ~PushStrlen() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) stream;
					int len = 0;
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						len = to_string(v.tint).length();
						break;
					case Any::Type::CHAR:
						len = 1;
						break;
					case Any::Type::STRING:
						len = ::strlen(v.tstring);
						break;
					}
					data.stk.pop();
					data.stk.push(len);
				}
			};

			struct PushLiteralInt : public Operation {
				const int literal;

				PushLiteralInt(int literal) : literal(literal) {}
				virtual ~PushLiteralInt() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) stream;
					data.stk.push(this->literal);
				}
			};

			struct PopSetDynamic : public Operation {
				const unsigned int arg;

				PopSetDynamic(unsigned int arg) : arg(arg) {
					if (arg > 25) {
						throw prtty::PrttyError("arg value must be between 0-25 (inclusive)");
					}
				}
				virtual ~PopSetDynamic() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) stream;
					data.dparm[this->arg] = data.stk.top();
					data.stk.pop();
				}
			};

			struct PopSetStatic : public Operation {
				const unsigned int arg;

				PopSetStatic(unsigned int arg) : arg(arg) {
					if (arg > 25) {
						throw prtty::PrttyError("arg value must be between 0-25 (inclusive)");
					}
				}
				virtual ~PopSetStatic() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) stream;
					data.sparm[this->arg] = data.stk.top();
					data.stk.pop();
				}
			};

			struct PushDynamic : public Operation {
				const unsigned int arg;

				PushDynamic(unsigned int arg) : arg(arg) {
					if (arg > 25) {
						throw prtty::PrttyError("arg value must be between 0-25 (inclusive)");
					}
				}
				virtual ~PushDynamic() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) stream;
					data.stk.push(data.dparm[this->arg]);
				}
			};

			struct PushStatic : public Operation {
				const unsigned int arg;

				PushStatic(unsigned int arg) : arg(arg) {
					if (arg > 25) {
						throw prtty::PrttyError("arg value must be between 0-25 (inclusive)");
					}
				}
				virtual ~PushStatic() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) stream;
					data.stk.push(data.sparm[this->arg]);
				}
			};

			struct PopWriteString : public Operation {
				virtual ~PopWriteString() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						stream << to_string(v.tint);
						break;
					case Any::Type::CHAR:
						stream << string(1, v.tchar);
						break;
					case Any::Type::STRING:
						stream << string(v.tstring);
						break;
					}
					data.stk.pop();
				}
			};

			struct PopWriteInt : public Operation {
				virtual ~PopWriteInt() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						stream << to_string(v.tint);
						break;
					case Any::Type::CHAR:
						stream << to_string((int) v.tchar);
						break;
					case Any::Type::STRING:
						stream << to_string(0);
						break;
					}
					data.stk.pop();
				}
			};

			struct PopWriteOct : public PopWriteInt {
				virtual ~PopWriteOct() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					stream << oct;
					this->PopWriteInt::operator()(stream, data, citr);
					stream << dec;
				}
			};

			struct PopWriteHex : public PopWriteInt {
				virtual ~PopWriteHex() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					stream << hex;
					this->PopWriteInt::operator()(stream, data, citr);
					stream << dec;
				}
			};

			struct PopWriteUHex : public PopWriteHex {
				virtual ~PopWriteUHex() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					stream << uppercase;
					this->PopWriteHex::operator()(stream, data, citr);
					stream << nouppercase;
				}
			};

			struct PopWriteChar : public Operation {
				virtual ~PopWriteChar() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						stream << string(1, (char) (v.tint & 0xFF));
						break;
					case Any::Type::CHAR:
						stream << string(1, v.tchar);
						break;
					case Any::Type::STRING:
						stream << string(1, v.tstring[0]);
						break;
					}
					data.stk.pop();
				}
			};

			struct IncrementFirstTwo : public Operation {
				virtual ~IncrementFirstTwo() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr) {
					(void) citr;
					(void) stream;
					if (data.params[0].type == Any::Type::INT) data.params[0].tint++;
					if (data.params[1].type == Any::Type::INT) data.params[1].tint++;
				}
			};
		}

		template <typename T, typename... Args>
		unique_ptr<T> mkunique(Args... args) {
			return unique_ptr<T>(new T(args...));
		}

		struct Sequence {
			template <typename... Args>
			void operator ()(Data &data, ostream &stream, Args... args) {
				data.session(args...);

				ios::fmtflags f(stream.flags());
				stream << dec;
				Operation::OpItr citr = this->ops.cbegin();
				Operation::OpItr cend = this->ops.cend();
				for (; citr != cend; citr++) {
					(**citr)(stream, data, citr);
				}
				stream.flags(f);
			}

			int nargs;
			vector<unique_ptr<Operation>> ops;

			static Sequence parse(const string fmt) {
				Sequence seq;
				seq.nargs = 0;
				size_t len = fmt.length();
				unique_ptr<char[]> literal(new char[len]);
				char lc = 0;
				char c = 0;
				int arg = 0;
				for (size_t i = 0; i < len; i++) {
					c = fmt[i];

					if (c == '%') {
						c = fmt[++i];

						if (c == '%') {
							goto addLiteral;
						}

						if (lc) {
							seq.ops.push_back(mkunique<op::StringLiteral>(string(literal.get(), lc)));
							lc = 0;
						}

						switch (c) {
						case 'c':
							seq.ops.push_back(mkunique<op::PopWriteChar>());
							break;
						case 's':
							seq.ops.push_back(mkunique<op::PopWriteString>());
							break;
						case 'd':
							seq.ops.push_back(mkunique<op::PopWriteInt>());
							break;
						case 'x':
							seq.ops.push_back(mkunique<op::PopWriteHex>());
							break;
						case 'X':
							seq.ops.push_back(mkunique<op::PopWriteUHex>());
							break;
						case 'o':
							seq.ops.push_back(mkunique<op::PopWriteOct>());
							break;
						case 'p':
							c = fmt[++i];
							if (c < '1' || c > '9') {
								throw prtty::PrttyError("push argument escape (%p) must be followed by a number between 1-9 (inclusive): %p" + string(1, c));
							}

							arg = c - '1';
							seq.nargs = seq.nargs > arg ? seq.nargs : arg;
							seq.ops.push_back(mkunique<op::PushArg>(arg));
							break;
						case 'P':
							c = fmt[++i];
							if (c >= 'a' && c<= 'z') {
								seq.ops.push_back(mkunique<op::PopSetDynamic>(c - 'a'));
							} else if (c >= 'A' && c <= 'Z') {
								seq.ops.push_back(mkunique<op::PopSetStatic>(c - 'A'));
							} else {
								throw prtty::PrttyError("set dynamic/static variable escape (%P) must be followed by a character within a-Z or A-Z: %P" + string(1, c));
							}
							break;
						case 'g':
							c = fmt[++i];
							if (c >= 'a' && c<= 'z') {
								seq.ops.push_back(mkunique<op::PushDynamic>(c - 'a'));
							} else if (c >= 'A' && c <= 'Z') {
								seq.ops.push_back(mkunique<op::PushStatic>(c - 'A'));
							} else {
								throw prtty::PrttyError("get dynamic/static variable escape (%g) must be followed by a character within a-Z or A-Z: %P" + string(1, c));
							}
							break;
						case '\'':
							c = fmt[++i];
							if (fmt[++i] != '\'') {
								throw prtty::PrttyError("character literal was unterminated (expected '): %'" + string(1, c) + "'");
							}

							seq.ops.push_back(mkunique<op::CharLiteral>(c));
							break;
						case '{': {
							++i;
							if (fmt[i] == '}') break;

							arg = 0;

							int sign = 1;
							if (fmt[i] == '-') {
								sign = -1;
							}

							for (; i < len && fmt[i] != '}'; i++) {
								c = fmt[i];
								if (c < '0' || c > '9') {
									throw prtty::PrttyError("found invalid number literal");
								}
								arg *= 10;
								arg += fmt[i] - '0';
							}

							seq.ops.push_back(mkunique<op::IntLiteral>(arg * sign));
							break;
						}
						case 'l':
							seq.ops.push_back(mkunique<op::PushStrlen>());
							break;
						case 'i':
							seq.nargs = seq.nargs > 2 ? seq.nargs : 2;
							seq.ops.push_back(mkunique<op::IncrementFirstTwo>());
							break;
						default:
							throw prtty::PrttyError("invalid escape: %" + (c == 0 ? string("<EOF>") : string(1, c)));
						}
					} else {
					addLiteral:
						literal[lc++] = c;
					}
				}

				if (lc) {
					seq.ops.push_back(mkunique<op::StringLiteral>(string(literal.get(), lc)));
				}

				return seq;
			}
		};

		inline char hashCharacter(unsigned char c) {
			if (c < 10) {
				return c + '0';
			} else {
				return (c - 10) + 'a';
			}
		}

		void split(const string &s, char delim, vector<string> &elems) {
			stringstream ss(s);
			string item;
			while(getline(ss, item, delim)) {
				elems.push_back(item);
			}
		}
	}

	struct term {
		string id;
		vector<string> names;

		bool auto_left_margin;
		bool auto_right_margin;
		bool no_esc_ctlc;
		bool ceol_standout_glitch;
		bool eat_newline_glitch;
		bool erase_overstrike;
		bool generic_type;
		bool hard_copy;
		bool has_meta_key;
		bool has_status_line;
		bool insert_null_glitch;
		bool memory_above;
		bool memory_below;
		bool move_insert_mode;
		bool move_standout_mode;
		bool over_strike;
		bool status_line_esc_ok;
		bool dest_tabs_magic_smso;
		bool tilde_glitch;
		bool transparent_underline;
		bool xon_xoff;
		bool needs_xon_xoff;
		bool prtr_silent;
		bool hard_cursor;
		bool non_rev_rmcup;
		bool no_pad_char;
		bool non_dest_scroll_region;
		bool can_change;
		bool back_color_erase;
		bool hue_lightness_saturation;
		bool col_addr_glitch;
		bool cr_cancels_micro_mode;
		bool has_print_wheel;
		bool row_addr_glitch;
		bool semi_auto_right_margin;
		bool cpi_changes_res;
		bool lpi_changes_res;

		int columns;
		int init_tabs;
		int lines;
		int lines_of_memory;
		int magic_cookie_glitch;
		int padding_baud_rate;
		int virtual_terminal;
		int width_status_line;
		int num_labels;
		int label_height;
		int label_width;
		int max_attributes;
		int maximum_windows;
		int max_colors;
		int max_pairs;
		int no_color_video;
		int buffer_capacity;
		int dot_vert_spacing;
		int dot_horz_spacing;
		int max_micro_address;
		int max_micro_jump;
		int micro_col_size;
		int micro_line_size;
		int number_of_pins;
		int output_res_char;
		int output_res_line;
		int output_res_horz_inch;
		int output_res_vert_inch;
		int print_rate;
		int wide_char_size;
		int buttons;
		int bit_image_entwining;
		int bit_image_type;

		string back_tab;
		string bell;
		string carriage_return;
		string change_scroll_region;
		string clear_all_tabs;
		string clear_screen;
		string clr_eol;
		string clr_eos;
		string column_address;
		string command_character;
		string cursor_address;
		string cursor_down;
		string cursor_home;
		string cursor_invisible;
		string cursor_left;
		string cursor_mem_address;
		string cursor_normal;
		string cursor_right;
		string cursor_to_ll;
		string cursor_up;
		string cursor_visible;
		string delete_character;
		string delete_line;
		string dis_status_line;
		string down_half_line;
		string enter_alt_charset_mode;
		string enter_blink_mode;
		string enter_bold_mode;
		string enter_ca_mode;
		string enter_delete_mode;
		string enter_dim_mode;
		string enter_insert_mode;
		string enter_secure_mode;
		string enter_protected_mode;
		string enter_reverse_mode;
		string enter_standout_mode;
		string enter_underline_mode;
		string erase_chars;
		string exit_alt_charset_mode;
		string exit_attribute_mode;
		string exit_ca_mode;
		string exit_delete_mode;
		string exit_insert_mode;
		string exit_standout_mode;
		string exit_underline_mode;
		string flash_screen;
		string form_feed;
		string from_status_line;
		string init_1string;
		string init_2string;
		string init_3string;
		string init_file;
		string insert_character;
		string insert_line;
		string insert_padding;
		string key_backspace;
		string key_catab;
		string key_clear;
		string key_ctab;
		string key_dc;
		string key_dl;
		string key_down;
		string key_eic;
		string key_eol;
		string key_eos;
		string key_f0;
		string key_f1;
		string key_f10;
		string key_f2;
		string key_f3;
		string key_f4;
		string key_f5;
		string key_f6;
		string key_f7;
		string key_f8;
		string key_f9;
		string key_home;
		string key_ic;
		string key_il;
		string key_left;
		string key_ll;
		string key_npage;
		string key_ppage;
		string key_right;
		string key_sf;
		string key_sr;
		string key_stab;
		string key_up;
		string keypad_local;
		string keypad_xmit;
		string lab_f0;
		string lab_f1;
		string lab_f10;
		string lab_f2;
		string lab_f3;
		string lab_f4;
		string lab_f5;
		string lab_f6;
		string lab_f7;
		string lab_f8;
		string lab_f9;
		string meta_off;
		string meta_on;
		string newline;
		string pad_char;
		string parm_dch;
		string parm_delete_line;
		string parm_down_cursor;
		string parm_ich;
		string parm_index;
		string parm_insert_line;
		string parm_left_cursor;
		string parm_right_cursor;
		string parm_rindex;
		string parm_up_cursor;
		string pkey_key;
		string pkey_local;
		string pkey_xmit;
		string print_screen;
		string prtr_off;
		string prtr_on;
		string repeat_char;
		string reset_1string;
		string reset_2string;
		string reset_3string;
		string reset_file;
		string restore_cursor;
		string row_address;
		string save_cursor;
		string scroll_forward;
		string scroll_reverse;
		string set_attributes;
		string set_tab;
		string set_window;
		string tab;
		string to_status_line;
		string underline_char;
		string up_half_line;
		string init_prog;
		string key_a1;
		string key_a3;
		string key_b2;
		string key_c1;
		string key_c3;
		string prtr_non;
		string char_padding;
		string acs_chars;
		string plab_norm;
		string key_btab;
		string enter_xon_mode;
		string exit_xon_mode;
		string enter_am_mode;
		string exit_am_mode;
		string xon_character;
		string xoff_character;
		string ena_acs;
		string label_on;
		string label_off;
		string key_beg;
		string key_cancel;
		string key_close;
		string key_command;
		string key_copy;
		string key_create;
		string key_end;
		string key_enter;
		string key_exit;
		string key_find;
		string key_help;
		string key_mark;
		string key_message;
		string key_move;
		string key_next;
		string key_open;
		string key_options;
		string key_previous;
		string key_print;
		string key_redo;
		string key_reference;
		string key_refresh;
		string key_replace;
		string key_restart;
		string key_resume;
		string key_save;
		string key_suspend;
		string key_undo;
		string key_sbeg;
		string key_scancel;
		string key_scommand;
		string key_scopy;
		string key_screate;
		string key_sdc;
		string key_sdl;
		string key_select;
		string key_send;
		string key_seol;
		string key_sexit;
		string key_sfind;
		string key_shelp;
		string key_shome;
		string key_sic;
		string key_sleft;
		string key_smessage;
		string key_smove;
		string key_snext;
		string key_soptions;
		string key_sprevious;
		string key_sprint;
		string key_sredo;
		string key_sreplace;
		string key_sright;
		string key_srsume;
		string key_ssave;
		string key_ssuspend;
		string key_sundo;
		string req_for_input;
		string key_f11;
		string key_f12;
		string key_f13;
		string key_f14;
		string key_f15;
		string key_f16;
		string key_f17;
		string key_f18;
		string key_f19;
		string key_f20;
		string key_f21;
		string key_f22;
		string key_f23;
		string key_f24;
		string key_f25;
		string key_f26;
		string key_f27;
		string key_f28;
		string key_f29;
		string key_f30;
		string key_f31;
		string key_f32;
		string key_f33;
		string key_f34;
		string key_f35;
		string key_f36;
		string key_f37;
		string key_f38;
		string key_f39;
		string key_f40;
		string key_f41;
		string key_f42;
		string key_f43;
		string key_f44;
		string key_f45;
		string key_f46;
		string key_f47;
		string key_f48;
		string key_f49;
		string key_f50;
		string key_f51;
		string key_f52;
		string key_f53;
		string key_f54;
		string key_f55;
		string key_f56;
		string key_f57;
		string key_f58;
		string key_f59;
		string key_f60;
		string key_f61;
		string key_f62;
		string key_f63;
		string clr_bol;
		string clear_margins;
		string set_left_margin;
		string set_right_margin;
		string label_format;
		string set_clock;
		string display_clock;
		string remove_clock;
		string create_window;
		string goto_window;
		string hangup;
		string dial_phone;
		string quick_dial;
		string tone;
		string pulse;
		string flash_hook;
		string fixed_pause;
		string wait_tone;
		string user0;
		string user1;
		string user2;
		string user3;
		string user4;
		string user5;
		string user6;
		string user7;
		string user8;
		string user9;
		string orig_pair;
		string orig_colors;
		string initialize_color;
		string initialize_pair;
		string set_color_pair;
		string set_foreground;
		string set_background;
		string change_char_pitch;
		string change_line_pitch;
		string change_res_horz;
		string change_res_vert;
		string define_char;
		string enter_doublewide_mode;
		string enter_draft_quality;
		string enter_italics_mode;
		string enter_leftward_mode;
		string enter_micro_mode;
		string enter_near_letter_quality;
		string enter_normal_quality;
		string enter_shadow_mode;
		string enter_subscript_mode;
		string enter_superscript_mode;
		string enter_upward_mode;
		string exit_doublewide_mode;
		string exit_italics_mode;
		string exit_leftward_mode;
		string exit_micro_mode;
		string exit_shadow_mode;
		string exit_subscript_mode;
		string exit_superscript_mode;
		string exit_upward_mode;
		string micro_column_address;
		string micro_down;
		string micro_left;
		string micro_right;
		string micro_row_address;
		string micro_up;
		string order_of_pins;
		string parm_down_micro;
		string parm_left_micro;
		string parm_right_micro;
		string parm_up_micro;
		string select_char_set;
		string set_bottom_margin;
		string set_bottom_margin_parm;
		string set_left_margin_parm;
		string set_right_margin_parm;
		string set_top_margin;
		string set_top_margin_parm;
		string start_bit_image;
		string start_char_set_def;
		string stop_bit_image;
		string stop_char_set_def;
		string subscript_characters;
		string superscript_characters;
		string these_cause_cr;
		string zero_motion;
		string char_set_names;
		string key_mouse;
		string mouse_info;
		string req_mouse_pos;
		string get_mouse;
		string set_a_foreground;
		string set_a_background;
		string pkey_plab;
		string device_type;
		string code_set_init;
		string set0_des_seq;
		string set1_des_seq;
		string set2_des_seq;
		string set3_des_seq;
		string set_lr_margin;
		string set_tb_margin;
		string bit_image_repeat;
		string bit_image_newline;
		string bit_image_carriage_return;
		string color_names;
		string define_bit_image_region;
		string end_bit_image_region;
		string set_color_band;
		string set_page_length;
		string display_pc_char;
		string enter_pc_charset_mode;
		string exit_pc_charset_mode;
		string enter_scancode_mode;
		string exit_scancode_mode;
		string pc_term_options;
		string scancode_escape;
		string alt_scancode_esc;
		string enter_horizontal_hl_mode;
		string enter_left_hl_mode;
		string enter_low_hl_mode;
		string enter_right_hl_mode;
		string enter_top_hl_mode;
		string enter_vertical_hl_mode;
		string set_a_attributes;
		string set_pglen_inch;
	};

	term get(string termname, string basePath) {
		ifstream dbf;
		string dbPath = basePath + "/" + string(1, termname[0]) + "/" + termname;
		dbf.open(dbPath, ios::binary);
		if (!dbf) {
			char hash[2];
			unsigned char firstchar = termname[0];
			hash[0] = impl::hashCharacter((firstchar & 0xF0) >> 4);
			hash[1] = impl::hashCharacter(firstchar & 0x0F);

			dbPath = basePath + "/" + string(&hash[0], 2) + "/" + termname;
			dbf.open(dbPath, ios::binary);

			if (!dbf) {
				throw PrttyError("could not load database for terminal: " + termname + " (from base search path: " + basePath + ")");
			}
		}

		term result;
		result.id = termname;

		uint8_t _u8;
		uint16_t _u16;

		#define READ_U8() dbf.read((char *)&_u8, 1)
		#define READ_U16() dbf.read((char *)&_u16, 2)

		// magic number
		READ_U16();
		if (_u16 != 0x11A) {
			throw PrttyError("terminal description file has invalid magic number");
		}

		READ_U16();
		size_t nameSize = _u16;
		READ_U16();
		size_t boolSize = _u16;
		READ_U16();
		size_t numCount = _u16;
		READ_U16();
		size_t offCount = _u16;
		READ_U16();
		// size_t strSize = _u16;

		unique_ptr<char[]> fullNameStr(new char[nameSize]);
		dbf.read(fullNameStr.get(), nameSize);
		impl::split(string(fullNameStr.get(), nameSize - 1), '|', result.names);

		bool *bools = (bool *) &(result.auto_left_margin);
		for (size_t i = 0; i < boolSize; i++) {
			READ_U8();
			bools[i] = (bool) _u8;
		}

		// align to short
		if ((dbf.tellg() % 2) == 1) {
			dbf.ignore(1);
		}

		int *ints = (int *) &(result.columns);
		for (size_t i = 0; i < numCount; i++) {
			READ_U16();
			ints[i] = (int) _u16;
		}

		char buf[4096];
		size_t offset = offCount * 2;
		auto loadString = [&dbf, &offset, &_u16, &buf]() -> string {
			READ_U16();
			offset -= 2;
			if (_u16 == (uint16_t) -1) {
				return "";
			}
			auto curpos = dbf.tellg();
			dbf.ignore(offset + _u16);
			dbf.getline(&buf[0], 4096, '\0');
			dbf.seekg(curpos);
			return string(&buf[0]);
		};

		auto loadStrings = [&](string *start, size_t num) {
			for (size_t i = 0; i < num; i++) {
				start[i] = loadString();
			}
		};

		loadStrings((string *) &(result.back_tab), 394);

		#undef READ_U8
		#undef READ_U16
		return result;
	}

	term get(string termname) {
		return get(termname, "/usr/share/terminfo");
	}

	term get() {
		char *termname = getenv("TERM");
		return get(termname ? termname : "dumb");
	}

}

#endif
