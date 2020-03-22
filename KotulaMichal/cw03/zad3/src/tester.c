#include <common.h>
#include <stdbool.h>
#include <wait.h>

void multiply(matrix a, matrix b, matrix* c)
{
    for(int row = 0; row < a.size.rows; row++)
    {
        for(int column = 0; column < b.size.columns; column++)
        {
            c->m[row][column] = 0;
            for(int k = 0; k < a.size.columns; k++)
            {
                c->m[row][column] += a.m[row][k] * b.m[k][column];
            }
        }
    }
}

int main(int argc, char** argv)
{
    int fh_mts = open(argv[1], O_RDONLY);
    multiplication_triples triples = load_multiplication_triples(fh_mts);
    close(fh_mts);

    for(int i=0;i<triples.size;i++)
    {
        int fh_a = open(triples.triples[i].a, O_RDONLY);
        int fh_b = open(triples.triples[i].b, O_RDONLY);
        int fh_c = open(triples.triples[i].c, O_RDONLY);

        if(fh_a == -1 || fh_b == -1 || fh_c == -1)
        {
            printf("Test failed: %s*%s = %s <- output doesn't exist\n", triples.triples[i].a, triples.triples[i].b, triples.triples[i].c);
            continue;
        }

        matrix a = load_matrix(fh_a);
        matrix b = load_matrix(fh_b);
        matrix c = load_matrix(fh_c);

        close(fh_a);
        close(fh_b);
        close(fh_c);

        matrix c_valid = create_matrix(a.size.rows, b.size.columns);

        multiply(a,b,&c_valid);

        if(c.size.rows != c_valid.size.rows || c.size.columns != c_valid.size.columns)
        {
            printf("Test failed: %s invalid size\n", triples.triples[i].c);
            continue;
        }

        bool same = true;
        int ir = 0;
        int ic = 0;

        for(int row=0;row<c.size.rows;row++)
        {
            for(int column=0;column<c.size.columns;column++)
            {
                if(c_valid.m[row][column] != c.m[row][column])
                {
                    ir = row;
                    ic = column;
                    printf("%i != %i\n", c_valid.m[row][column], c.m[row][column]);
                    same = false;
                }
            }
        }

        if(!same)
        {
            printf("Test failed: invalid value at %i, %i\n", ir, ic);
        }
    }

    free_multiplication_triples(&triples);

    return 0;
}