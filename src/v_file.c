#include "v_file.h"

V_file* v_file__new(void)
{
    V_file* v_file = malloc(sizeof(V_file));
    if(v_file == NULL)
    {
        return NULL;
    }

    v_file->pathname = malloc(PATH_MAX);
    if(v_file->pathname == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for a directory.\n");
        return NULL;
    }

    // Pointer to pointers, so an address size is required.
    v_file->lines = malloc(sizeof(Line));
    if(v_file->lines == NULL)
    {
        fprintf(stderr, "Can't alloc a memory a array with lines.\n");
        return NULL;
    }
    v_file->chars_amount      = 0;
    v_file->lines_amount      = 0;
    v_file->mirrored_cursor_x = 0;
    v_file->mirrored_cursor_y = 0;
    V_FILE__ACTUAL_LINE.len   = 0;
    v_file->fname_len         = 0;
    v_file->fname[0]          = '\0';
    v_file->fname_copy[0]     = '\0';
    v_file->pathname[0]       = '\0';
    v_file->basename[0]       = '\0';
    v_file->extension[0]      = '\0';
    v_file->esc_seq_on_input  = false;

    V_FILE__ACTUAL_LINE.txt = malloc(V_FILE__BASIC_MEMBLK);
    if(V_FILE__ACTUAL_LINE.txt == NULL)
    {
        fprintf(stderr, "Can't allocate a memory for a first line.\n");
        return NULL;
    }
    return v_file;
}

bool buffer__init(V_file* const v_file)
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
    v_file->chars_amount     = 0;
    v_file->lines_amount     = 0;
    v_file->mirrored_cursor_x     = 0;
    v_file->mirrored_cursor_y     = 0;
    V_FILE__ACTUAL_LINE.len  = 0;
    v_file->fname_len        = 0;
    v_file->fname[0]         = '\0';
    v_file->fname_copy[0]    = '\0';
    v_file->pathname[0]      = '\0';
    v_file->basename[0]      = '\0';
    v_file->extension[0]     = '\0';
    v_file->esc_seq_on_input = false;

    V_FILE__ACTUAL_LINE.txt = malloc(V_FILE__BASIC_MEMBLK);
    if(V_FILE__ACTUAL_LINE.txt == NULL)
    {
        fprintf(stderr, "Can't allocate a memory for a first line.\n");
        return false;
    }
    return true;
}

Line* v_file__get_last_line(const V_file* const this)
{
    return &this->lines[this->lines_amount];
}

size_t v_file__get_cursor_y(const V_file* const this)
{
    return this->lines_amount - this->mirrored_cursor_y;
}

Line* v_file__get_actual_line(const V_file* const this)
{
    return &this->lines[v_file__get_cursor_y(this)];
}

size_t v_file__get_cursor_x(const V_file* const this)
{
    return v_file__get_actual_line(this)->len - this->mirrored_cursor_x;
}

char v_file__get_actual_char(const V_file* const this)
{
    return v_file__get_actual_line(this)->txt[v_file__get_cursor_x(this)];
}

bool v_file__is_cursor_x_scrolled(const V_file* const this)
{
    return this->mirrored_cursor_x > 0;
}

bool v_file__is_cursor_y_scrolled(const V_file* const this)
{
    return this->mirrored_cursor_y > 0;
}

void v_file__delete(V_file* const v_file)
{
    for(size_t line_i = 0; line_i <= v_file->lines_amount; line_i++)
    {
        if(v_file->lines[line_i].txt != NULL)
        {
            free(v_file->lines[line_i].txt);
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
