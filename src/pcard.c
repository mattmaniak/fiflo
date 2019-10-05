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
    int          pcard_offset = (int) ((unsigned) config->Pcard_w.value
                                       - line_len - SIZE__I);

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
        else if((line_txt[line_len] == '\n') && ((line_len + SIZE__I) == pcard_w)
                && ((int) line_len >= -ui->pcard_delta_x))
        {
            ui__colorize(config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
            putchar(' ');
        }
        ui__colorize(0);
    }
    else if(ui->txtarea_w > (ui->pcard_delta_x + (int) pcard_w - SIZE__LF))
    {
        // A line is hidden. Punched card is not.
        if(-ui->pcard_delta_x > (int) line_len)
        {
            printf("%*s", ui->pcard_delta_x + (int) pcard_w - SIZE__LF, " ");
        }
        else // A line is visible and shifted but not punched card.
        {
            printf("%*s", (int) (pcard_w - line_len - SIZE__LF), " ");
        }
        ui__colorize(config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
        putchar(' ');
        ui__colorize(0);
    }
}

void pcard__print_after_nothing(const Config* const config,
                                const Ui* const ui)
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
