#include <common.h>

void right_pad(char* text, char with, size_t from, size_t to)
{
    memset(text + from, with,  to - from);
}

int load_element(int file_handle)
{
    char element[MATRIX_ENTRY_SIZE + 1];
    element[MATRIX_ENTRY_SIZE]=0;
    read(file_handle, element, MATRIX_ENTRY_SIZE);
    lseek(file_handle, 1, SEEK_CUR); // skip separator space
    return atoi(element);
}
void save_element(int file_handle, int value)
{
    char element[MATRIX_ENTRY_SIZE];
    right_pad(element, ' ', snprintf(element, MATRIX_ENTRY_SIZE, "%i", value), MATRIX_ENTRY_SIZE);
    write(file_handle, element, MATRIX_ENTRY_SIZE);
}
void save_separator(int file_handle, char s)
{
    write(file_handle, &s, 1);
}
void goto_element(int file_handle, matrix_descriptor md, int row, int column)
{
    lseek(file_handle, row*(MATRIX_ENTRY_SIZE+1)*md.columns + (MATRIX_ENTRY_SIZE+1)*column + (MATRIX_ENTRY_SIZE+1)*2,SEEK_SET);
}

matrix_descriptor load_matrix_descriptor(int file_handle)
{
    lseek(file_handle, 0, SEEK_SET);
    matrix_descriptor md;
    md.rows = load_element(file_handle);
    md.columns = load_element(file_handle);
    return md;
}

matrix create_matrix(int rows, int columns)
{
    matrix m;
    m.size.rows = rows;
    m.size.columns = columns;
    m.m = calloc(m.size.rows, sizeof(int*));

    for(int r = 0; r < m.size.rows; r++) {
        m.m[r] = calloc(m.size.columns, sizeof(int));
    }

    return m;
}
matrix load_matrix(int file_handle)
{
    matrix m;
    m.size = load_matrix_descriptor(file_handle);

    m.m = calloc(m.size.rows, sizeof(int*));

    for(int r = 0; r < m.size.rows; r++)
    {
        m.m[r] = calloc(m.size.columns, sizeof(int));

        for(int c = 0; c < m.size.columns; c++)
        {
            goto_element(file_handle, m.size, r, c);
            m.m[r][c] = load_element(file_handle);
        }
    }

    return m;
}
void save_matrix(int file_handle, matrix m)
{
    lseek(file_handle, 0, SEEK_SET);
    save_element(file_handle, m.size.rows);
    save_separator(file_handle, '\t');
    save_element(file_handle, m.size.columns);
    save_separator(file_handle, '\n');

    for(int r = 0; r < m.size.rows; r++)
    {
        for(int c = 0; c < m.size.columns - 1; c++)
        {
            save_element(file_handle, m.m[r][c]);
            save_separator(file_handle, '\t');
        }
        save_element(file_handle, m.m[r][m.size.columns - 1]);
        save_separator(file_handle, '\n');
    }
}
void prepare_file_for_matrix(int file_handle, matrix_descriptor md)
{
    lseek(file_handle, 0, SEEK_SET);

    lseek(file_handle, 0, SEEK_SET);
    save_element(file_handle, md.rows);
    save_separator(file_handle, '\t');
    save_element(file_handle, md.columns);
    save_separator(file_handle, '\n');

    for(int r = 0; r < md.rows; r++)
    {
        for(int c = 0; c < md.columns - 1; c++)
        {
            save_element(file_handle, 0);
            save_separator(file_handle, '\t');
        }
        save_element(file_handle, 0);
        save_separator(file_handle, '\n');
    }
}

void save_matrix_element(int file_handle, matrix m, int row, int column)
{
    goto_element(file_handle, m.size, row, column);
    save_element(file_handle, m.m[row][column]);
}
int load_matrix_element(int file_handle, matrix_descriptor md, int row, int column)
{
    goto_element(file_handle, md, row, column);
    return load_element(file_handle);
}
void free_matrix(matrix* m) {
    for (int r = 0; r < m->size.rows; r++) {
        free(m->m[r]);
    }
    free(m->m);
    m->m = NULL;
}

void free_multiplication_triple(multiplication_triple* mt)
{
    free(mt->a);
    free(mt->b);
    free(mt->c);
    mt->a = NULL;
    mt->b = NULL;
    mt->c = NULL;
}
multiplication_triples load_multiplication_triples(int file_handle)
{
    int size = lseek(file_handle, 0, SEEK_END);
    char* file = calloc(size, sizeof(char));
    lseek(file_handle, 0, SEEK_SET);
    read(file_handle, file, size);

    char a[FILENAME_MAX];
    char b[FILENAME_MAX];
    char c[FILENAME_MAX];

    int triples = 0;
    char* from = file;

    while(sscanf(from, "%s %s %s", a, b, c) == 3)
    {
        from += strlen(a) + strlen(b) + strlen(c) + 2;
        triples++;
    }

    multiplication_triples mts;
    mts.triples = calloc(triples, sizeof(multiplication_triple));
    mts.size = triples;

    from = file;
    for(int i=0;i<triples;i++)
    {
        sscanf(from, "%s %s %s", a, b, c);
        from += strlen(a) + strlen(b) + strlen(c) + 3;
        mts.triples[i].a = calloc(strlen(a) + 1, sizeof(char));
        mts.triples[i].b = calloc(strlen(b) + 1, sizeof(char));
        mts.triples[i].c = calloc(strlen(c) + 1, sizeof(char));
        strcpy(mts.triples[i].a, a);
        strcpy(mts.triples[i].b, b);
        strcpy(mts.triples[i].c, c);
    }

    return mts;
}
void free_multiplication_triples(multiplication_triples* mt) {
    for(int i=0;i<mt->size;i++)
    {
        free_multiplication_triple(mt->triples + i);
    }
    free(mt->triples);
    mt = NULL;
};