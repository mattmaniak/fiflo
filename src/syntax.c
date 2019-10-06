#include "syntax.h"

bool syntax__load(Syntax* const syntax, const char* const extension)
{
    const char syntax_ext[]                      = ".fiflorc";
    char       syntax_fname[PATH_MAX + NAME_MAX] = "/usr/share/fiflo/";
    char       keyword[SYNTAX__MAX_KWRD_LEN];
    char       color[SYNTAX__MAX_KWRD_LEN];
    FILE*      File;

    syntax->keywords_amount = 0;

    strcat(syntax_fname, extension);
    strcat(syntax_fname, syntax_ext);

    if((extension == NULL) || (access(syntax_fname, R_OK) == -1))
    {
        return true;
    }
    File = fopen(syntax_fname, "r");
    if(File != NULL)
    {
        while(fscanf(File, "%s = %s", keyword, color) != EOF)
        {
            strncpy(syntax->Keywords[syntax->keywords_amount].keyword, keyword,
                    SYNTAX__MAX_KWRD_LEN);

            syntax->Keywords[syntax->keywords_amount].color =
            config__parse_value(color);

            if(syntax->keywords_amount++ >= SYNTAX__MAX_KWRDS_IN_FILE)
            {
                break;
            }
        }
        syntax->keywords_amount--;
        syntax__sort(syntax);

        if(fclose(File) == EOF)
        {
            fprintf(stderr, "Failed to close a syntax File.\n");
            return false;
        }
    }
    return true;
}

void syntax__sort(Syntax* const syntax)
{
    syntax__Keyword Tmp_keyword;

    // Iterate through all keywords.
    for(size_t keyword_i = 0; keyword_i < syntax->keywords_amount; keyword_i++)
    {
        // Set a value of a further keyword to compare.
        for(size_t shift_i = keyword_i + SIZE__NEXT;
            shift_i < syntax->keywords_amount; shift_i++)
        {
            // Swap if a next keyword is longer.
            if(strlen(syntax->Keywords[keyword_i].keyword)
               < strlen(syntax->Keywords[shift_i].keyword))
            {
                Tmp_keyword = syntax->Keywords[keyword_i];

                syntax->Keywords[keyword_i]  = syntax->Keywords[shift_i];
                syntax->Keywords[shift_i] = Tmp_keyword;
            }
        }
    }
}

size_t syntax__paint_word(const Syntax* const syntax,
                          const Config* const config, Line* Line,
                          const size_t end_ch_i, size_t ch_i)
{
    const char* const str_to_print_addr = &Line->txt[ch_i];
    const size_t      pcard_w           = (size_t) config->Pcard_w.value;
    bool              keyword_ignored      = false;
    size_t            end_paint_i;

    if(syntax->keywords_amount <= 0)
    {
        return ch_i;
    }
    for(size_t keyword_i = 0; keyword_i <= syntax->keywords_amount; keyword_i++)
    {
        if(strstr(str_to_print_addr, syntax->Keywords[keyword_i].keyword)
           == str_to_print_addr)
        {
            ui__colorize(syntax->Keywords[keyword_i].color);

            end_paint_i = (size_t) strlen(syntax->Keywords[keyword_i].keyword)
                          + ch_i;

            if(end_paint_i == 0)
            {
                end_paint_i  = ch_i;
                keyword_ignored = true;
            }

            // Breaks a word if the end of a terminal is achieved.
            if(end_paint_i > end_ch_i)
            {
                end_paint_i = end_ch_i;
            }
            for(; ch_i < end_paint_i; ch_i++)
            {
                if(ch_i == (pcard_w - SIZE__I))
                {
                    ui__colorize(config->Color_ui.value
                                 + ANSI__BG_COLOR_OFFSET);
                    ui__colorize(syntax->Keywords[keyword_i].color);
                }
                putchar(Line->txt[ch_i]);
                if(ch_i == (pcard_w - SIZE__I))
                {
                    // Reset the background color after punch card.
                    ui__colorize(0);
                    ui__colorize(syntax->Keywords[keyword_i].color);
                }
            }
            // Not a last char, so don't hide a next.
            if(!keyword_ignored && (ch_i < end_ch_i))
            {
                ch_i--; // An other char will be printed. Make it visible.
            }
            break;
        }
    }
    return ch_i;
}
