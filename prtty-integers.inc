#ifndef PRTTY_DO_FIRST_INTEGER
#	define PRTTY_DO_FIRST_INTEGER PRTTY_DO_INTEGER
#endif
#ifndef PRTTY_DO_LAST_INTEGER
#	define PRTTY_DO_LAST_INTEGER PRTTY_DO_INTEGER
#endif

#ifndef PRTTY_FIRST_INTEGER
#	define PRTTY_FIRST_INTEGER columns
#endif
PRTTY_DO_INTEGER(columns)
PRTTY_DO_INTEGER(init_tabs)
PRTTY_DO_INTEGER(lines)
PRTTY_DO_INTEGER(lines_of_memory)
PRTTY_DO_INTEGER(magic_cookie_glitch)
PRTTY_DO_INTEGER(padding_baud_rate)
PRTTY_DO_INTEGER(virtual_terminal)
PRTTY_DO_INTEGER(width_status_line)
PRTTY_DO_INTEGER(num_labels)
PRTTY_DO_INTEGER(label_height)
PRTTY_DO_INTEGER(label_width)
PRTTY_DO_INTEGER(max_attributes)
PRTTY_DO_INTEGER(maximum_windows)
PRTTY_DO_INTEGER(max_colors)
PRTTY_DO_INTEGER(max_pairs)
PRTTY_DO_INTEGER(no_color_video)
PRTTY_DO_INTEGER(buffer_capacity)
PRTTY_DO_INTEGER(dot_vert_spacing)
PRTTY_DO_INTEGER(dot_horz_spacing)
PRTTY_DO_INTEGER(max_micro_address)
PRTTY_DO_INTEGER(max_micro_jump)
PRTTY_DO_INTEGER(micro_col_size)
PRTTY_DO_INTEGER(micro_line_size)
PRTTY_DO_INTEGER(number_of_pins)
PRTTY_DO_INTEGER(output_res_char)
PRTTY_DO_INTEGER(output_res_line)
PRTTY_DO_INTEGER(output_res_horz_inch)
PRTTY_DO_INTEGER(output_res_vert_inch)
PRTTY_DO_INTEGER(print_rate)
PRTTY_DO_INTEGER(wide_char_size)
PRTTY_DO_INTEGER(buttons)
PRTTY_DO_INTEGER(bit_image_entwining)
PRTTY_DO_INTEGER(bit_image_type)

#define PRTTY_NUM_INTEGERS 33

#undef PRTTY_DO_INTEGER
#undef PRTTY_DO_FIRST_INTEGER
#undef PRTTY_DO_LAST_INTEGER
