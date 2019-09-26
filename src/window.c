#include "window.h"

term_t window__receive_term_sz(const char axis)
{
    const int sz_max = USHRT_MAX;
    const int h_min  = UI__UBAR_SZ + SIZE__LN + UI__MAX_LBAR_H; // TODO: DYNAMIC
    const int w_min  = UI__GIT_LOGO_W + SIZE__SPACE + UI__GIT_BRANCH_MIN_W
                       + SIZE__SPACE + V_FILE__STATUS_MAX
                       + UI__HORIZONTAL_PADDING;

    const struct winsize Term_win;

    /* "TIOCGWINSZ request to the stdout descriptor. &term is required by that
        specific device (stdout)." */
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &Term_win) == -1)
    {
        fprintf(stderr, "Can't get a Term_win's size.\n");
        return 0;
    }

    // Terminal size check.
    if((Term_win.ws_col < w_min) || (Term_win.ws_row < h_min))
    {
        fprintf(stderr, "Minimal Term_win's size: %ux%u.\n", w_min, h_min);
        return 0;
    }
    else if((Term_win.ws_col > sz_max) || (Term_win.ws_row > sz_max))
    {
        fprintf(stderr, "Maximum Term_win's size: %ux%u.\n", sz_max, sz_max);
        return 0;
    }

    switch(axis)
    {
    case 'w':
        return Term_win.ws_col;

    case 'h':
        return Term_win.ws_row;
    }
    return 0;
}

void window__flush(void)
{
    const term_t term_h = window__receive_term_sz('h');

    // Restore to a lowest left corner and clean a lowest line.
    ANSI__RESTORE_CURSOR_POS();
    ANSI__CLEAN_WHOLE_LN();

    // Then from move up and clean next lines till a window ends.
    for(term_t ln = UI__LBAR_SZ; ln < term_h; ln++)
    {
        ANSI__CURSOR_UP(1);
        ANSI__CLEAN_WHOLE_LN();
    }
    fflush(NULL);
}

void window__fill(const V_file_t* const V_file, const Conf_t* const Config,
                  const Ui_t* const Ui)
{
    // Fill an empty area below a text to adjust a position the lower bar.
    if((V_file->ln_amount + SIZE__I) < (size_t) Ui->txtarea_h)
    {
        for(size_t line = V_file->ln_amount;
            line < (size_t) (Ui->txtarea_h - UI__LBAR_SZ); line++)
        {
            UI__WRAP_LN();
            pcard__print_after_nothing(Config, Ui);
        }
    }
    // Else the lower bar will by positioned by a text.
}

void window__set_cursor_pos(const V_file_t* const V_file,
                            const Mod_t* const Modes, const Ui_t* const Ui)
{
    // Set by default to a filename edit.
    term_t move_right = (term_t) (UI__LEFT_PADDING + V_file->fname_len);
    term_t move_up    = (term_t) (Ui->win_h - UI__LBAR_SZ);

    if(move_right >= Ui->win_w)
    {
        move_right = (term_t) (Ui->win_w - SIZE__CURSOR);
    }

    // Cursor is pushed right by the lower bar. Move it back.
    ANSI__CURSOR_LEFT(Ui->win_w);
    ANSI__SAVE_CURSOR_POS();

    if(!Modes->live_fname_edit)
    {
        if(V_FILE__ACTUAL_LN.len < Ui->txtarea_w)
        {
            // No horizontal scrolling.
            move_right = (term_t) (Ui->ln_num_len + V_FILE__CURSOR_X);
        }
        else if((V_FILE__ACTUAL_LN.len - Ui->txtarea_w)
                >= V_file->cursor_rev_x)
        {
            /* Last Ui->txtarea_w chars are seen. Current line is scrolled,
               not cursor. */
            move_right = (term_t) (Ui->win_w - SIZE__CURSOR);
        }
        else
        {
            // Text is scrolled horizontally to a start. Cursor can be moved.
            move_right = (term_t) (Ui->ln_num_len + V_FILE__CURSOR_X);
        }
        move_up = (V_FILE__ACTUAL_LN_I < Ui->txtarea_h)
                  ? (term_t) (Ui->txtarea_h - V_FILE__ACTUAL_LN_I - SIZE__I
                     + Ui->lbar_h) : Ui->lbar_h;
    }
    ANSI__CURSOR_RIGHT(move_right);
    ANSI__CURSOR_UP(move_up);
}

bool window__render(const V_file_t* const V_file, const Conf_t* const Config,
                    const Mod_t* const Modes, const Syntax_t* const Syntax,
                    const size_t additional_argc_i, const size_t actual_file_i)
{
    char ln_num_str[16]; // Needed to count a length of a number.
    Ui_t Ui;

    sprintf(ln_num_str, "%lu", V_file->ln_amount + SIZE__I);

    if(((Ui.win_w = window__receive_term_sz('w')) == 0)
       || ((Ui.win_h = window__receive_term_sz('h')) == 0))
    {
        return false;
    }
    Ui.expanded_lbar_h = (term_t) (UI__LBAR_SZ + additional_argc_i + SIZE__I
                         + SIZE__LN);
    Ui.lbar_h          = (Modes->expanded_lbar) ? Ui.expanded_lbar_h
                         : UI__LBAR_SZ;

    Ui.ln_num_len = (term_t) (strlen(ln_num_str) + SIZE__SPACE
                    + UI__LEFT_PADDING);

    Ui.txtarea_w = (term_t) (Ui.win_w - Ui.ln_num_len);
    Ui.txtarea_h = (term_t) (Ui.win_h - UI__UBAR_SZ - Ui.lbar_h);

    Ui.pcard_delta_x = (int) (Ui.txtarea_w + V_file->cursor_rev_x
                       - V_FILE__ACTUAL_LN.len - SIZE__I);
    Ui.pcard_delta_x = (Ui.pcard_delta_x > 0) ? 0 : Ui.pcard_delta_x;

    ui__upper_bar(&V_file[actual_file_i], Config, &Ui);

    print__display_text(&V_file[actual_file_i], Config, Syntax, &Ui);
    window__fill(&V_file[actual_file_i], Config, &Ui);

    ui__lower_bar(V_file, Config, Modes, &Ui, additional_argc_i,
                  actual_file_i);
    window__set_cursor_pos(&V_file[actual_file_i], Modes, &Ui);

    return true;
}
