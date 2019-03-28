#include "buffer.h"
#include "config.h"
#include "shortcuts.h"
#include "modes.h"
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

    // Get the state of the standard input.
    if(tcgetattr(STDIN_FILENO, &old_term_params) == ERROR)
    {
        window__flush();
        fprintf(stderr, "Stdin attribiutes error. Pipe isn't supported.\n");
        return ERROR;
    }
    new_term_params = old_term_params;

    // Look that the options of below flags are negated.
    new_term_params.c_iflag &= ~(enable_xon);
    new_term_params.c_lflag &= ~(canonical_mode_on | echo_input | enable_sigs);

    /* Immediately set the state of the stdin to the *new_term_params. Use the
    new terminal I/O settings. */
    if(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_params) == ERROR)
    {
        window__flush();
        fprintf(stderr, "Can't set the terminal's raw mode. Type \"reset\".\n");
        return ERROR;
    }
    key = (char) getchar();

    // Immediately restore the state of the stdin (0) to the* new_term_params.
    if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_params) == ERROR)
    {
        window__flush();
        fprintf(stderr,
                "Can't restore the terminal's normal mode. Type \"reset\".\n");
        return ERROR;
    }
    return key;
}

void input__recognize_sequence(Buff_t* Buffer, Conf_t* Config, char* sequence)
{
    const size_t seq_max = 4;

    const char arrow_up[]    = "\033[A";
    const char arrow_down[]  = "\033[B";
    const char arrow_right[] = "\033[C";
    const char arrow_left[]  = "\033[D";

    if(!strcmp(sequence, arrow_up))
    {
        keys__arrow_up(Buffer);
        Buffer->chars_sequence = false;
    }
    else if(!strcmp(sequence, arrow_down))
    {
        keys__arrow_down(Buffer);
        Buffer->chars_sequence = false;
    }
    else if(!strcmp(sequence, arrow_right))
    {
        keys__arrow_right(Buffer, Config);
        Buffer->chars_sequence = false;
    }
    else if(!strcmp(sequence, arrow_left))
    {
        keys__arrow_left(Buffer, Config);
        Buffer->chars_sequence = false;
    }
    else if(strlen(sequence) >= seq_max)
    {
        Buffer->chars_sequence = false;
    }

#ifdef DEBUG_INPUT
    printf("cursor_rev_x %u, cursor_rev_y %u.\n", Buffer->cursor_rev_x,
           Buffer->cursor_rev_y);
#endif

}

bool input__parse_key(Buff_t* Buffer, Conf_t* Config, Mod_t* Modes, char key)
{
    static char  chars_sequence[SEQ_MAX];
    static idx_t char_idx;

    if(key == CTRL_LEFT_BRACKET)
    {
        Buffer->chars_sequence = true;

#ifdef DEBUG_INPUT
        Buffer->chars_sequence = false;
#endif

        char_idx = 0;
    }
    if(Buffer->chars_sequence)
    {
        chars_sequence[char_idx] = key;
        if(char_idx < (SEQ_MAX - NUL_SZ))
        {
            char_idx++;
        }
        chars_sequence[char_idx] = '\0';

        input__recognize_sequence(Buffer, Config, chars_sequence);
        if(!Buffer->chars_sequence)
        {
            char_idx = 0;
        }
    }
    else if(Modes->live_fname_edit)
    {
        file__live_edit_name(Buffer, Config, Modes, key);
    }
    else
    {
        return keys__key_action(Buffer, Config, Modes, key);
    }
    return true;
}
