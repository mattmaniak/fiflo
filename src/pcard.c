#include "pcard.h"

void pcard__print_at_offset(const Config* const config, const size_t char_i)
{
    if(char_i == (size_t) (config->Pcard_w.value - SIZE__I))
    {
        ui__colorize(config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
    }
}

void pcard__print_after_txt(const Config* const config, const Ui* const ui,
                            const char* const line_txt, const size_t line_len)
{
    const size_t pcard_w      = (size_t) config->Pcard_w.value;
    int          pcard_offset = (int) ((unsigned) pcard_w - line_len
                                       - SIZE__I);

    if(ui->txtarea_w >= config->Pcard_w.value)
    {
        if((line_len + SIZE__I) < pcard_w)
        {
            if((int) line_len < -ui->pcard_delta_x)
            {
                pcard_offset = config->Pcard_w.value + ui->pcard_delta_x
                               - SIZE__I;
            }
            if(pcard_offset > 0)
            {
                printf("%*s", pcard_offset, " ");
                ui__colorize(config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
                putchar(' ');
            }
            else if(pcard_offset == 0)
            {
                ui__colorize(config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
                putchar(' ');
            }
        }
        // The Linefeed highlighting.
        else if((line_len >= pcard_w) && (pcard_w == ui->txtarea_w)
                && (ui->pcard_delta_x == 0))
        {
            ui__colorize(config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
            putchar(' ');
        }
        ui__colorize(0);
    }
    else if(((line_len + ui->pcard_delta_x) > 0)
            && (-ui->pcard_delta_x <= pcard_w))
    {
        const bool offset_smaller_than_txtarea = (ui->pcard_delta_x + (int) pcard_w - SIZE__LF) < ui->txtarea_w;
        // A line is hidden. Punched card is not.
        if(-ui->pcard_delta_x > (int) (line_len)) // TODO
        {
            if(((-ui->pcard_delta_x + SIZE__I) < pcard_w)
               && offset_smaller_than_txtarea)
            {
                printf("%*s", ui->pcard_delta_x + (int) pcard_w - SIZE__LF, " ");
                // printf("[%d %d]", ui->pcard_delta_x + (int) pcard_w - SIZE__LF);
                putchar('!');
            }
            else if((-ui->pcard_delta_x + SIZE__I) == pcard_w)
            {
                putchar('X');
            }
        }
        // A line is scrolled and partially visible.
        else if((ui->pcard_delta_x < 0) && (line_len < ui->txtarea_w)
                && offset_smaller_than_txtarea)
        {
            printf("%*s", pcard_offset, " ");
            putchar('*');
        }
        // ui__colorize(config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
        // ui__colorize(0);
    }
    // Transition.
    // else if(-ui->pcard_delta_x == line_len)
    // {
    //     printf("%*s", ui->txtarea_w - line_len, " ");
    //     putchar('X');
    // }
}

void pcard__print_after_nothing(const Config* const config, const Ui* const ui)
{
    const int pcard_w = config->Pcard_w.value;

    if((ui->txtarea_w >= (ui->pcard_delta_x + pcard_w))
       && (-ui->pcard_delta_x < pcard_w))
    {
        printf("%*s",
               ui->line_num_len + ui->pcard_delta_x + pcard_w - SIZE__I, " ");

        ui__colorize(config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
        putchar(' ');
        ui__colorize(0);
    }
}
