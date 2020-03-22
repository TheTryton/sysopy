#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>

#define MATRIX_ENTRY_SIZE 8
#define DESCRIPTOR_OFFSET (MATRIX_ENTRY_SIZE*2 + 2)

typedef struct matrix_descriptor
{
    int rows;
    int columns;
} matrix_descriptor;

typedef struct matrix
{
    int** m;
    matrix_descriptor size;
} matrix;

typedef struct multiplication_triple {
    char* a;
    char* b;
    char* c;
} multiplication_triple;

typedef struct multiplication_triples {
    multiplication_triple* triples;
    int size;
} multiplication_triples;

void right_pad(char* text, char with, size_t from, size_t to);

int load_element(int file_handle);
void save_element(int file_handle, int value);
void save_separator(int file_handle, char s);
void goto_element(int file_handle, matrix_descriptor md, int row, int column);

matrix_descriptor load_matrix_descriptor(int file_handle);

matrix create_matrix(int rows, int columns);
matrix load_matrix(int file_handle);
void save_matrix(int file_handle, matrix m);
void prepare_file_for_matrix(int file_handle, matrix_descriptor md);

void save_matrix_element(int file_handle, matrix m, int row, int column);
int load_matrix_element(int file_handle, matrix_descriptor md, int row, int column);
void free_matrix(matrix* m);

void free_multiplication_triple(multiplication_triple* mt);
multiplication_triples load_multiplication_triples(int file_handle);
void free_multiplication_triples(multiplication_triples* mt);
