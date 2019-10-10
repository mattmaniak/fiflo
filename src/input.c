#include "input.h"

char input__getch(void)
{
    const unsigned int canonical_mode_on = ICANON;
    const unsigned int echo_input        = ECHO;
    const unsigned int enable_sigs       = ISIG;
    const unsigned int enable_xon        = IXON;

    struct termios old_term_params;
    struct termios new_term_params;

    char key;

    // Get a state of the standard input.
    if(tcgetattr(STDIN_FILENO, &old_term_params) == -1)
    {
        window__flush();
        fprintf(stderr, "Stdin attribiutes error. Pipe isn't supported.\n");

        return (char) -1;
    }
    new_term_params = old_term_params;

    // Notice that options of below flags are negated.
    new_term_params.c_iflag &= ~enable_xon;
    new_term_params.c_lflag &= ~(canonical_mode_on | echo_input | enable_sigs);

    /* Immediately set a state of the stdin to the *new_term_params. Use the
       new terminal I/O settings. */
    if(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_params) == -1)
    {
        window__flush();
        fprintf(stderr, "Can't set a terminal's raw mode. Type \"reset\".\n");

        return (char) -1;
    }
    key = (char) getchar();

    // Immediately restore the state of the stdin (0) to the* new_term_params.
    if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_params) == -1)
    {
        window__flush();
        fprintf(stderr,
                "Can't restore a terminal's normal mode. Type \"reset\".\n");
        return (char) -1;
    }
    return key;
}

void input__recognize_sequence(V_file* const v_file,
                               const Config* const config,
                               const char* const sequence,
                               size_t* const file_i)
{
    const size_t seq_len_max = 6;

    const char arrow_up[]    = "\033[A";
    const char arrow_down[]  = "\033[B";
    const char arrow_right[] = "\033[C";
    const char arrow_left[]  = "\033[D";

    const char ctrl_arrow_up[]    = "\033[1;5A";
    const char ctrl_arrow_down[]  = "\033[1;5B";
    const char ctrl_arrow_right[] = "\033[1;5C";
    const char ctrl_arrow_left[]  = "\033[1;5D";

    const char ctrl_f1[] = "\033[1;5P";
    const char ctrl_f2[] = "\033[1;5Q";
    const char ctrl_f3[] = "\033[1;5R";
    const char ctrl_f4[] = "\033[1;5S";

    if(!strcmp(sequence, arrow_up))
    {
        keys__arrow_up(v_file);
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, arrow_down))
    {
        keys__arrow_down(v_file);
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, arrow_right))
    {
        keys__arrow_right(v_file, config);
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, arrow_left))
    {
        keys__arrow_left(v_file, config);
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_arrow_up)) // Scroll to the beginning now.
    {
        keys__ctrl_arrow_up(v_file);
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_arrow_down)) // Scroll to the end of file.
    {
        keys__ctrl_arrow_down(v_file);
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_arrow_right))
    {
        keys__ctrl_arrow_right(v_file);
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_arrow_left))
    {
        keys__ctrl_arrow_left(v_file);
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_f1))
    {
        *file_i = 0;
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_f2))
    {
        *file_i = 1;
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_f3))
    {
        *file_i = 2;
        v_file->esc_seq_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_f4))
    {
        *file_i = 3;
        v_file->esc_seq_on_input = false;
    }
    // Other cases that block an input for "seq_len_max" chars.
    else if(strlen(sequence) >= seq_len_max)
    {
        v_file->esc_seq_on_input = false;
    }

#ifdef DEBUG_INPUT
    printf("mirrored_cursor_x %u, mirrored_cursor_y %u.\n",
           v_file->mirrored_cursor_x, v_file->mirrored_cursor_y);
#endif

}

bool input__parse_key(V_file* const v_file, const Config* const config,
                      Modes* const modes, size_t* const file_i, const char key)
{
    static char   ch_sequence[INPUT__SEQ_MAX];
    static size_t char_i;

    if((key == ASCII__CTRL_LEFT_BRACKET) && !modes->live_fname_edit)
    {
        char_i                     = 0;
        v_file->esc_seq_on_input = true;

#ifdef DEBUG_INPUT
        v_file->esc_seq_on_input = false;
#endif

    }
    if(v_file->esc_seq_on_input)
    {
        ch_sequence[char_i] = key;
        if(char_i < (INPUT__SEQ_MAX - SIZE__NUL))
        {
            char_i++;
        }
        ch_sequence[char_i] = '\0';
        input__recognize_sequence(v_file, config, ch_sequence, file_i);

        if(!v_file->esc_seq_on_input)
        {
            char_i = 0;
        }
    }
    else if(modes->live_fname_edit)
    {
        edit__filename(v_file, config, modes, key);
    }
    else
    {
        return input__parse_char(v_file, config, modes, key);
    }
    return true;
}

bool input__parse_char(V_file* const v_file, const Config* const config,
                    Modes* const modes, const char ch)
{
    switch(ch)
    {
    default:
        return input__printable_char(v_file, ch);

    case '\t':
        return keys__tab(v_file, config, modes);

    case ASCII__BACKSPACE:
        return keys__backspace(v_file, config, modes);

    case ASCII__CTRL_Q:
        return false;

    case ASCII__CTRL_S:
        return file_io__save(v_file, config);

    case ASCII__CTRL_BACKSLASH:
        modes->expanded_lbar = !modes->expanded_lbar;
        break;

    case ASCII__CTRL_D:
        return edit__delete_line(v_file);

    case ASCII__CTRL_O:
        modes->live_fname_edit = true;
    }
    return true;
}

bool input__printable_char(V_file* const v_file, const char ch)
{

#ifdef DEBUG_KEYS
    const bool char_is_allowed = true;
#else
    const bool char_is_allowed = (ch == '\0') || (ch == '\t') || (ch == '\n')
                                 || (ch >= 32);
#endif

    if(char_is_allowed)
    {
        if(v_file->chars_amount < V_FILE__CHAR_MAX)
        {
            v_file->chars_amount++;
            v_file_actual_line(v_file)->length++;

            if(!memory__extend_line(v_file, v_file_cursor_y(v_file)))
            {
                return false;
            }
            if(v_file__is_cursor_x_scrolled(v_file))
            {
                edit__shift_text_horizonally(v_file, 'r');
            }
            v_file_actual_line(v_file)->txt[
                v_file_cursor_x(v_file) - SIZE__NUL] = ch;

            v_file_actual_line(v_file)->txt[
                v_file_actual_line(v_file)->length] = '\0';

            // Initializing nul handler.
            if((ch == '\0') && !v_file__is_actual_line_empty(v_file))
            {
                v_file->chars_amount--;
                v_file_actual_line(v_file)->length--;
            }
            else if((ch == '\n') && !keys__linefeed(v_file))
            {
                return false;
            }
            if(ch != '\0')
            {
                V_FILE__SET_STATUS("edited");
            }
        }
        else
        {
            V_FILE__SET_STATUS("can't read or insert more chars");
        }
    }
    else
    {
        V_FILE__SET_STATUS("unsupported char(s)");
    }
    return true;
}
