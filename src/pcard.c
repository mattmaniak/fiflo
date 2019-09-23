#include "pcard.h"

void pcard__print_at_offset(const Conf_t* const Config, const idx_t ch_idx)
{
    if(ch_idx == (const idx_t) (Config->Pcard_w.value - SIZE__IDX))
    {
        ui__colorize(Config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
    }
}

void pcard__print_after_txt(const Conf_t* const Config, const Ui_t* const Ui,
                            const char* const ln_txt, const idx_t ln_len)
{
    const idx_t pcard_w      = (const idx_t) Config->Pcard_w.value;
    int         pcard_offset = (const int) ((const unsigned int)
                                            Config->Pcard_w.value - ln_len
                                            - SIZE__IDX);

    if(Ui->txtarea_w >= Config->Pcard_w.value)
    {
        if((ln_len + SIZE__IDX) < pcard_w)
        {
            if((const int) ln_len < -Ui->pcard_delta_x)
            {
                pcard_offset = Config->Pcard_w.value + Ui->pcard_delta_x
                               - SIZE__IDX;
            }
            if(pcard_offset > 0)
            {
                printf("%*s", pcard_offset, " ");
                ui__colorize(Config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
                putchar(' ');
            }
            else if(pcard_offset == 0)
            {
                ui__colorize(Config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
                putchar(' ');
            }
        }
        // The Linefeed highlighting.
        else if((ln_txt[ln_len] == '\n') && ((ln_len + SIZE__IDX) == pcard_w)
                && ((const int) ln_len >= -Ui->pcard_delta_x))
        {
                ui__colorize(Config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
                putchar(' ');
        }
        ui__colorize(0);
    }
}

void pcard__print_after_nothing(const Conf_t* const Config,
                                const Ui_t* const Ui)
{
    const int pcard_w = Config->Pcard_w.value;

    if((Ui->txtarea_w >= (Ui->pcard_delta_x + pcard_w))
       && (-Ui->pcard_delta_x < pcard_w))
    {
        printf("%*s", Ui->line_num_len + Ui->pcard_delta_x + pcard_w
               - SIZE__IDX, " ");

        ui__colorize(Config->Color_ui.value + ANSI__BG_COLOR_OFFSET);
        putchar(' ');
        ui__colorize(0);
    }
}
