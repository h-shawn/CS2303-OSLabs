#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

int **matrixA, **matrixB, **result;
int n;
struct timeval stop, start;

void matrix_multiply()
{
    gettimeofday(&start, NULL);
    int i, j, k;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            for (k = 0; k < n; k++)
            {
                result[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
    gettimeofday(&stop, NULL);
}

void write_matrix_to_file(int **matrix, int n, char *filename)
{
    FILE *fp = fopen(filename, "w");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf(fp, "%d ", matrix[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    // Default input and output file names
    char *input_file = "data.in";
    char *output_file = "data.out";

    if (argc > 3)
    {
        printf("Usage: %s <InputFile> <OutputFile>.\n", argv[0]);
        exit(-1);
    }
    else if (argc == 3)
    {
        input_file = argv[1];
        output_file = argv[2];
    }
    // Read file
    FILE *f = fopen(input_file, "r");

    if (!f)
    {
        printf("File could not be opened. Exiting.\n");
        return 1;
    }
    fscanf(f, "%d", &n);
    matrixA = (int **)malloc(n * sizeof(int *));
    matrixB = (int **)malloc(n * sizeof(int *));
    result = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        matrixA[i] = (int *)malloc(n * sizeof(int));
        matrixB[i] = (int *)malloc(n * sizeof(int));
        result[i] = (int *)malloc(n * sizeof(int));
    }
    // Read Matrices
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int tmp;
            fscanf(f, "%d", &tmp);
            matrixA[i][j] = tmp;
            matrixB[i][j] = tmp;
            result[i][j] = 0;
        }
    }

    matrix_multiply();
    double time1, time2;
    time1 = start.tv_sec + (start.tv_usec / 1000000.0);
    time2 = stop.tv_sec + (stop.tv_usec / 1000000.0);

    printf("Time used: %f seconds\n", time2 - time1);

    write_matrix_to_file(result, n, output_file);
    for (int i = 0; i < n; i++)
    {
        free(result[i]);
        free(matrixA[i]);
        free(matrixB[i]);
    }
    free(result);
    free(matrixA);
    free(matrixB);
    fclose(f);
    return 0;
}