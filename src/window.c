#include "window.h"

term_t window__receive_term_sz(const char axis)
{
    const unsigned sz_max = USHRT_MAX;
    const unsigned h_min  = UI__UBAR_SZ + SIZE__LINE + UI__MAX_LBAR_H; // TODO: DYNAMIC
    const unsigned w_min  = UI__GIT_LOGO_W + SIZE__SPACE + UI__GIT_BRANCH_MIN_W
                            + SIZE__SPACE + V_FILE__STATUS_MAX
                            + UI__HORIZONTAL_PADDING;

    const struct winsize terminal;

    /* "TIOCGWINSZ request to the stdout descriptor. &term is required by that
        specific device (stdout)." */
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == -1)
    {
        fprintf(stderr, "Can't get a terminal's size.\n");
        return 0;
    }

    // Terminal size check.
    if((terminal.ws_col < w_min) || (terminal.ws_row < h_min))
    {
        fprintf(stderr, "Min. terminal size: %ux%u.\n", w_min, h_min);
        return 0;
    }
    else if((terminal.ws_col > sz_max) || (terminal.ws_row > sz_max))
    {
        fprintf(stderr, "Max. terminal size: %ux%u.\n", sz_max, sz_max);
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
    const term_t term_h = window__receive_term_sz('h');

    // Restore to a lowest left corner and clean a lowest line.
    ANSI__RESTORE_CURSOR_POS();
    ANSI__CLEAN_WHOLE_LINE();

    // Then from move up and clean next lines till a window ends.
    for(term_t line = UI__LBAR_SZ; line < term_h; line++)
    {
        ANSI__CURSOR_UP(1);
        ANSI__CLEAN_WHOLE_LINE();
    }
    fflush(NULL);
}

void window__fill(const V_file* const v_file, const Config* const config,
                  const Ui* const ui)
{
    const size_t line_to_fill = (size_t) ui->txtarea_h - UI__LBAR_SZ;

    // Fill an empty area below a text to adjust a position the lower bar.
    if((v_file->lines_amount + SIZE__I) < (size_t) ui->txtarea_h)
    {
        for(size_t line = v_file->lines_amount; line < line_to_fill; line++)
        {
            UI__WRAP_LINE();
            pcard__print_after_nothing(config, ui);
        }
    }
    // Else the lower bar will by positioned by a text.
}

void window__set_cursor_pos(const V_file* const v_file,
                            const Modes* const modes, const Ui* const ui)
{
    // Set by default to a filename edit.
    term_t move_right = (term_t) (UI__LEFT_PADDING + v_file->fname_len);
    term_t move_up    = (term_t) (ui->win_h - UI__LBAR_SZ);

    if(move_right >= ui->win_w)
    {
        move_right = (term_t) (ui->win_w - SIZE__CURSOR);
    }

    // Cursor is pushed right by the lower bar. Move it back.
    ANSI__CURSOR_LEFT(ui->win_w);
    ANSI__SAVE_CURSOR_POS();

    if(!modes->live_fname_edit)
    {
        if(V_FILE__ACTUAL_LINE.len < ui->txtarea_w)
        {
            // No horizontal scrolling.
            move_right = (term_t) (ui->line_num_len + V_FILE__CURSOR_X);
        }
        else if((V_FILE__ACTUAL_LINE.len - ui->txtarea_w)
                >= v_file->mirrored_cursor_x)
        {
            /* Last ui->txtarea_w chars are seen. Current line is scrolled,
               not cursor. */
            move_right = (term_t) (ui->win_w - SIZE__CURSOR);
        }
        else
        {
            // Text is scrolled horizontally to a start. Cursor can be moved.
            move_right = (term_t) (ui->line_num_len + V_FILE__CURSOR_X);
        }
        move_up = (V_FILE__ACTUAL_LINE_I < ui->txtarea_h)
                  ? (term_t) (ui->txtarea_h - V_FILE__ACTUAL_LINE_I - SIZE__I
                     + ui->lbar_h) : ui->lbar_h;
    }
    ANSI__CURSOR_RIGHT(move_right);
    ANSI__CURSOR_UP(move_up);
}

bool window__render(const V_file* const v_file, const Config* const config,
                    const Modes* const modes, const Syntax* const syntax,
                    const size_t additional_argc_i, const size_t actual_file_i)
{
    char line_num_str[16]; // Needed to count a length of a number.
    Ui   ui;

    sprintf(line_num_str, "%lu", v_file[actual_file_i].lines_amount + SIZE__I);

    if(((ui.win_w = window__receive_term_sz('w')) == 0)
       || ((ui.win_h = window__receive_term_sz('h')) == 0))
    {
        return false;
    }
    ui.expanded_lbar_h = (term_t) (UI__LBAR_SZ + additional_argc_i + SIZE__I);
    ui.lbar_h          = (modes->expanded_lbar) ? ui.expanded_lbar_h
                         : UI__LBAR_SZ;

    ui.line_num_len = (term_t) (strlen(line_num_str) + SIZE__SPACE
                    + UI__LEFT_PADDING);

    ui.txtarea_w = (term_t) (ui.win_w - ui.line_num_len);
    ui.txtarea_h = (term_t) (ui.win_h - UI__UBAR_SZ - ui.lbar_h);

    ui.pcard_delta_x = (int) (ui.txtarea_w + v_file[actual_file_i].mirrored_cursor_x
                       - V_FILE__ACTUAL_LINE.len - SIZE__I);
    ui.pcard_delta_x = (ui.pcard_delta_x > 0) ? 0 : ui.pcard_delta_x;

    ui__upper_bar(&v_file[actual_file_i], config, &ui);

    print__display_text(&v_file[actual_file_i], config, syntax, &ui);
    window__fill(&v_file[actual_file_i], config, &ui);

    ui__lower_bar(v_file, config, modes, &ui, additional_argc_i,
                  actual_file_i);
    window__set_cursor_pos(&v_file[actual_file_i], modes, &ui);

    return true;
}
