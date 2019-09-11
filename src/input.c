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
    if(tcgetattr(STDIN_FILENO, &old_term_params) == ERROR)
    {
        window__flush();
        fprintf(stderr, "Stdin attribiutes error. Pipe isn't supported.\n");

        return ERROR;
    }
    new_term_params = old_term_params;

    // Notice that options of below flags are negated.
    new_term_params.c_iflag &= ~enable_xon;
    new_term_params.c_lflag &= ~(canonical_mode_on | echo_input | enable_sigs);

    /* Immediately set a state of the stdin to the *new_term_params. Use the
       new terminal I/O settings. */
    if(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_params) == ERROR)
    {
        window__flush();
        fprintf(stderr, "Can't set a terminal's raw mode. Type \"reset\".\n");

        return ERROR;
    }
    key = (char) getchar();

    // Immediately restore the state of the stdin (0) to the* new_term_params.
    if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_params) == ERROR)
    {
        window__flush();
        fprintf(stderr,
                "Can't restore a terminal's normal mode. Type \"reset\".\n");
        return ERROR;
    }
    return key;
}

void input__recognize_sequence(Buff_t* Buffer, const Conf_t* const Config,
                               const char* const sequence, size_t* file_idx)
{
    const size_t seq_length_max = 6;

    const char arrow_up[]    = "\033[A";
    const char arrow_down[]  = "\033[B";
    const char arrow_right[] = "\033[C";
    const char arrow_left[]  = "\033[D";

    const char ctrl_arrow_up[]    = "\033[1;5A";
    const char ctrl_arrow_down[]  = "\033[1;5B";
    const char ctrl_arrow_right[] = "\033[1;5C";
    const char ctrl_arrow_left[]  = "\033[1;5D";

    const char ctrl_f1[]  = "\033[1;5P";
    const char ctrl_f2[]  = "\033[1;5Q";
    const char ctrl_f3[]  = "\033[1;5R";
    const char ctrl_f4[]  = "\033[1;5S";

    if(!strcmp(sequence, arrow_up))
    {
        keys__arrow_up(Buffer);
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, arrow_down))
    {
        keys__arrow_down(Buffer);
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, arrow_right))
    {
        keys__arrow_right(Buffer, Config);
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, arrow_left))
    {
        keys__arrow_left(Buffer, Config);
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_arrow_up)) // Scroll to the beginning now.
    {
        keys__ctrl_arrow_up(Buffer);
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_arrow_down)) // Scroll to the end of file.
    {
        keys__ctrl_arrow_down(Buffer);
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_arrow_right))
    {
        keys__ctrl_arrow_right(Buffer);
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_arrow_left))
    {
        keys__ctrl_arrow_left(Buffer);
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_f1))
    {
        *file_idx = 0;
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_f2))
    {
        *file_idx = 1;
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_f3))
    {
        *file_idx = 2;
        Buffer->escape_sequence_on_input = false;
    }
    else if(!strcmp(sequence, ctrl_f4))
    {
        *file_idx = 3;
        Buffer->escape_sequence_on_input = false;
    }
    // Other cases that block an input for "seq_length_max" chars.
    else if(strlen(sequence) >= seq_length_max)
    {
        Buffer->escape_sequence_on_input = false;
    }

#ifdef DEBUG_INPUT
    printf("cursor_rev_x %u, cursor_rev_y %u.\n", Buffer->cursor_rev_x,
           Buffer->cursor_rev_y);
#endif

}

bool input__parse_key(Buff_t* Buffer, const Conf_t* const Config, Mod_t* Modes,
                      size_t* file_idx, const char key)
{
    static char  chars_sequence[INPUT__SEQ_MAX];
    static idx_t char_idx;

    if((key == KEYS__CTRL_LEFT_BRACKET) && !Modes->live_fname_edit)
    {
        Buffer->escape_sequence_on_input = true;

#ifdef DEBUG_INPUT
        Buffer->escape_sequence_on_input = false;
#endif

        char_idx = 0;
    }
    if(Buffer->escape_sequence_on_input)
    {
        chars_sequence[char_idx] = key;
        if(char_idx < (INPUT__SEQ_MAX - NUL_SZ))
        {
            char_idx++;
        }
        chars_sequence[char_idx] = '\0';
        input__recognize_sequence(Buffer, Config, chars_sequence, file_idx);

        if(!Buffer->escape_sequence_on_input)
        {
            char_idx = 0;
        }
    }
    else if(Modes->live_fname_edit)
    {
        edit__filename(Buffer, Config, Modes, key);
    }
    else
    {
        return chars__parse_char(Buffer, Config, Modes, key);
    }
    return true;
}
