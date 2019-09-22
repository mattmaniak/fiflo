#include "pcard.h"

void pcard__print_at_offset(const Conf_t* const Config, const idx_t ch_idx)
{
    if(ch_idx == (const idx_t) (Config->Punch_card_width.value - IDX))
    {
        ui__colorize(Config->Color_ui.value + 10);
    }
}

void pcard__print_after_txt(const Conf_t* const Config, const Ui_t* const Ui,
                            const char* const ln_txt, const idx_t ln_len)
{
    int pcard_offset = Config->Punch_card_width.value - ln_len - IDX;

    if(Ui->txtarea_w >= Config->Punch_card_width.value)
    {
        if(((ln_len + IDX) < (const idx_t) Config->Punch_card_width.value))
        {
            if((const int) ln_len < -Ui->pcard_delta_x)
            {
                pcard_offset = Config->Punch_card_width.value
                               + Ui->pcard_delta_x - IDX;
            }
            if(pcard_offset > 0)
            {
                printf("%*s", pcard_offset, " ");
                ui__colorize(Config->Color_ui.value + 10);
                putchar(' ');
            }
            else if(pcard_offset == 0)
            {
                ui__colorize(Config->Color_ui.value + 10);
                putchar(' ');
            }
        }
        // The Linefeed highlighting.
        else if((ln_txt[ln_len] == '\n')
                && ((ln_len + IDX) == (const idx_t) Config->Punch_card_width.value)
                && ((const int) ln_len >= -Ui->pcard_delta_x))
        {
                ui__colorize(Config->Color_ui.value + 10);
                putchar(' ');
        }
        ui__colorize(0);
    }
}

void pcard__print_after_nothing(const Conf_t* const Config,
                                const Ui_t* const Ui)
{
    if((Ui->txtarea_w >= (Ui->pcard_delta_x + Config->Punch_card_width.value))
       && (-Ui->pcard_delta_x < Config->Punch_card_width.value))
    {
        printf("%*s", Ui->line_num_len + Ui->pcard_delta_x
               + Config->Punch_card_width.value - IDX, " ");

        ui__colorize(Config->Color_ui.value + 10);
        putchar(' ');
        ui__colorize(0);
    }
}
