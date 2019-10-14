#include "v_file.h"

bool v_file__init(V_file* const v_file)
{
    v_file->pathname = malloc(PATH_MAX);
    if(v_file->pathname == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for a directory.\n");
        return false;
    }

    // Pointer to pointers, so an address size is required.
    v_file->lines = malloc(sizeof(Line));
    if(v_file->lines == NULL)
    {
        fprintf(stderr, "Can't alloc a memory a array with lines.\n");
        return false;
    }
    v_file->chars_amount             = 0;
    v_file->lines_amount             = 0;
    v_file->mirrored_cursor_x        = 0;
    v_file->mirrored_cursor_y        = 0;
    v_file__actual_line(v_file)->len = 0;
    v_file->fname_len                = 0;
    v_file->fname[0]                 = '\0';
    v_file->fname_copy[0]            = '\0';
    v_file->pathname[0]              = '\0';
    v_file->basename[0]              = '\0';
    v_file->extension[0]             = '\0';
    v_file->esc_seq_on_input         = false;

    v_file__actual_line(v_file)->txt = malloc(V_FILE__BASIC_MEMBLOCK);
    if(v_file__actual_line(v_file)->txt == NULL)
    {
        fprintf(stderr, "Can't allocate a memory for a first line.\n");
        return false;
    }
    return true;
}

void v_file__delete(V_file* const v_file)
{
    for(size_t ln_i = 0; ln_i <= v_file->lines_amount; ln_i++)
    {
        if(v_file->lines[ln_i].txt != NULL)
        {
            free(v_file->lines[ln_i].txt);
        }
    }
    if(v_file->lines != NULL)
    {
        free(v_file->lines);
    }
    if(v_file->pathname != NULL)
    {
        free(v_file->pathname);
    }
}

size_t v_file__cursor_x(const V_file* const this)
{
    return v_file__actual_line(this)->len - this->mirrored_cursor_x;
}

size_t v_file__cursor_y(const V_file* const this)
{
    return this->lines_amount - this->mirrored_cursor_y;
}

char* v_file__actual_char(const V_file* const this)
{
    return &v_file__actual_line(this)->txt[v_file__cursor_x(this)];
}

char* v_file__last_char_in_actual_line(const V_file* const this)
{
    return &v_file__actual_line(this)->txt[v_file__actual_line(this)->len];
}

char* v_file__last_char(const V_file* const this)
{
    return &v_file__last_line(this)->txt[v_file__last_line(this)->len];
}

Line* v_file__actual_line(const V_file* const this)
{
    return &this->lines[v_file__cursor_y(this)];
}

Line* v_file__prev_line(const V_file* const this)
{
    return &this->lines[v_file__cursor_y(this) - SIZE__PREV];
}

Line* v_file__last_line(const V_file* const this)
{
    return &this->lines[this->lines_amount];
}

bool v_file__is_cursor_x_scrolled(const V_file* const this)
{
    return this->mirrored_cursor_x > 0;
}

bool v_file__is_cursor_y_scrolled(const V_file* const this)
{
    return this->mirrored_cursor_y > 0;
}

bool v_file__is_actual_line_empty(const V_file* const this)
{
    return v_file__actual_line(this)->len == 0;
}

bool v_file__is_actual_line_first(const V_file* const this)
{
    return v_file__cursor_y(this) == 0;
}

bool v_file__is_cursor_at_top(const V_file* const this)
{
    return this->mirrored_cursor_y == this->lines_amount;
}

bool v_file__is_cursor_at_line_start(const V_file* const this)
{
    return this->mirrored_cursor_x == v_file__actual_line(this)->len;
}
