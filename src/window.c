#include "window.h"

term_t window__get_term_sz(const char axis)
{
    const int line_sz = 1;
    const int sz_max  = USHRT_MAX;
    const int h_min   = UI__UBAR_SZ + line_sz + UI__TOGGLED_LBAR_H;
    const int w_min   = UI__GIT_LOGO_W + SPACE_SZ + UI__GIT_BRANCH_MIN_W
                        + SPACE_SZ + BUFFER__STATUS_MAX + UI__HORIZONTAL_PADDING;

    const struct winsize terminal;

    /* TIOCGWINSZ request to the stdout descriptor. &term is required by that
    specific device (stdout). */
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == ERROR)
    {
        fprintf(stderr, "Can't get a terminal's size.\n");
        return 0;
    }

    // Terminal size check.
    if((terminal.ws_col < w_min) || (terminal.ws_row < h_min))
    {
        fprintf(stderr, "Minimal terminal's size: %ux%u.\n", w_min, h_min);
        return 0;
    }
    else if((terminal.ws_col > sz_max) || (terminal.ws_row > sz_max))
    {
        fprintf(stderr, "Maximum terminal's size: %ux%u.\n", sz_max, sz_max);
        return 0;
    }

    switch(axis)
    {
    case 'w':
        return terminal.ws_col;

    case 'h':
        return terminal.ws_row;
    }
    return 0;
}

void window__flush(void)
{
    const term_t term_h = window__get_term_sz('h');

    // Restore to a lowest left corner and clean a lowest line.
    ANSI_RESTORE_CURSOR_POS();
    ANSI_CLEAN_WHOLE_LINE();

    // Then from move up and clean next lines till a window ends.
    for(term_t line = UI__LBAR_SZ; line < term_h; line++)
    {
        ANSI_CURSOR_UP(1);
        ANSI_CLEAN_WHOLE_LINE();
    }
    fflush(NULL);
}

void window__fill(const Buff_t* const Buffer, const Conf_t* const Config,
                  const Ui_t* const Ui)
{
    // Fill an empty area below a text to adjust a position the lower bar.
    if((Buffer->lines_amount + IDX) < (const idx_t) Ui->textarea_h)
    {
        for(idx_t line = Buffer->lines_amount;
            line < (const idx_t) (Ui->textarea_h - UI__LBAR_SZ); line++)
        {
            WRAP_LINE();

            if(Ui->textarea_w >= (Ui->actual_punch_card_pos
                                  + Config->Punch_card_width.value))
            {
                if(-Ui->actual_punch_card_pos < Config->Punch_card_width.value)
                {
                    printf("%*s", Ui->line_num_len + Ui->actual_punch_card_pos
                           + Config->Punch_card_width.value - IDX, " ");

                    ui__colorize(&Config->Color_ui.value);
                    ANSI__INVERT();
                    putchar(' ');
                    ui__colorize(NULL);
                }
            }
        }
    }
    // Else the lower bar will by posed by a text.
}

void window__set_cursor_pos(const Buff_t* const Buffer,
                            const Mod_t* const Modes, const Ui_t* const Ui)
{
    // Set by default to a filename edit.
    term_t move_right = (const term_t) (UI__LEFT_PADDING + Buffer->fname_len);
    term_t move_up    = (const term_t) (Ui->win_h - UI__LBAR_SZ);

    if(move_right >= Ui->win_w)
    {
        move_right = (const term_t) (Ui->win_w - CURSOR_SZ);
    }

    // Cursor is pushed right by the lower bar. Move it back.
    ANSI_CURSOR_LEFT(Ui->win_w);
    ANSI_SAVE_CURSOR_POS();

    if(!Modes->live_fname_edit)
    {
        if(BUFFER__ACTUAL_LINE_LEN < Ui->textarea_w)
        {
            // No horizontal scrolling.
            move_right = (const term_t) (Ui->line_num_len + BUFFER__CURSOR_X);
        }
        else if((BUFFER__ACTUAL_LINE_LEN - Ui->textarea_w)
                >= Buffer->cursor_rev_x)
        {
            /* Last Ui->textarea_w chars are seen. Current line is scrolled,
               not cursor. */
            move_right = (const term_t) (Ui->win_w - CURSOR_SZ);
        }
        else
        {
            // Text is scrolled horizontally to a start. Cursor can be moved.
            move_right = (const term_t)
                         (Ui->line_num_len + BUFFER__CURSOR_X);
        }
        move_up = (BUFFER__ACTUAL_LINE_IDX < Ui->textarea_h) ?
                  (const term_t)
                  (Ui->textarea_h - BUFFER__ACTUAL_LINE_IDX - IDX + Ui->lbar_h)
                  : Ui->lbar_h;
    }
    ANSI_CURSOR_RIGHT(move_right);
    ANSI_CURSOR_UP(move_up);
}

bool window__render(const Buff_t* const Buffer, const Conf_t* const Config,
                    const Mod_t* const Modes, const Syntax_t* const Syntax,
                    const idx_t additional_argc_idx,
                    const idx_t actual_file_idx)
{
    char line_num_str[16]; // Needed to count a length of a number.
    Ui_t Ui;

    sprintf(line_num_str, "%u", Buffer->lines_amount + IDX);

    if(((Ui.win_w = window__get_term_sz('w')) == 0)
       || ((Ui.win_h = window__get_term_sz('h')) == 0))
    {
        return false;
    }
    Ui.expanded_lbar_h = UI__TOGGLED_LBAR_H;
    Ui.lbar_h          = (Modes->expanded_lbar) ? Ui.expanded_lbar_h
                         : UI__LBAR_SZ;

    Ui.line_num_len = (const term_t) (strlen(line_num_str) + SPACE_SZ
                      + UI__LEFT_PADDING);

    Ui.textarea_w = (const term_t) (Ui.win_w - Ui.line_num_len);
    Ui.textarea_h = (const term_t) (Ui.win_h - UI__UBAR_SZ - Ui.lbar_h);

    Ui.actual_punch_card_pos = Ui.textarea_w + Buffer->cursor_rev_x
                               - BUFFER__ACTUAL_LINE_LEN - IDX;
    if(Ui.actual_punch_card_pos > 0)
    {
        Ui.actual_punch_card_pos = 0;
    }
    ui__upper_bar(&Buffer[actual_file_idx], Config, &Ui);

    print__display_text(&Buffer[actual_file_idx], Config, Syntax, &Ui);
    window__fill(&Buffer[actual_file_idx], Config, &Ui);

    ui__lower_bar(Buffer, Config, Modes, &Ui, additional_argc_idx,
                  actual_file_idx);
    window__set_cursor_pos(&Buffer[actual_file_idx], Modes, &Ui);

    return true;
}
