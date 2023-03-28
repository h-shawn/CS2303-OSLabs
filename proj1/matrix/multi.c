#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

int **matrixA, **matrixB, **result;
int n, num_threads = 4;
struct timeval stop, start;

void *calculateRow(void *arg)
{
    long row = (long)arg;
    int limit = n / num_threads;
    int gap;
    for (int j = 0; j < n; j++)
    {
        for (int k = 0; k < n; k++)
        {
            for (int i = 0; i < limit; i++)
            {
                gap = num_threads * i;
                result[row + gap][j] += matrixA[row + gap][k] * matrixB[k][j];
            }
        }
    }
}

void matrix_multiply()
{
    gettimeofday(&start, NULL);
    pthread_t threads[num_threads];
    for (long i = 0; i < num_threads; i++)
    {
        int rc = pthread_create(&threads[i], NULL, calculateRow, (void *)i);
        if (rc)
        {
            printf("Error creating thread %ld\n", i);
            exit(-1);
        }
    }
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&stop, NULL);
}

void write_random_matrix_to_file(int **A, int **B, int **C, char *filename)
{
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%d\n", n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf(fp, "%d ", A[i][j]);
        }
        fprintf(fp, "\n");
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf(fp, "%d ", B[i][j]);
        }
        fprintf(fp, "\n");
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf(fp, "%d ", C[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void write_matrix_to_file(int **C, char *filename)
{
    FILE *fp = fopen(filename, "w");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf(fp, "%d ", C[i][j]);
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

    // input size to generate random matrixs
    if (argc == 2)
    {
        n = atoi(argv[1]);
        if (n % num_threads != 0)
        {
            printf("Number of threads should divide n.\n");
            exit(-1);
        }
        matrixA = (int **)malloc(n * sizeof(int *));
        matrixB = (int **)malloc(n * sizeof(int *));
        result = (int **)malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++)
        {
            matrixA[i] = (int *)malloc(n * sizeof(int));
            matrixB[i] = (int *)malloc(n * sizeof(int));
            result[i] = (int *)malloc(n * sizeof(int));
        }
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                matrixA[i][j] = rand() % 10;
                matrixB[i][j] = rand() % 10;
                result[i][j] = 0;
            }
        }

        matrix_multiply();

        double time1, time2;
        time1 = start.tv_sec + (start.tv_usec / 1000000.0);
        time2 = stop.tv_sec + (stop.tv_usec / 1000000.0);

        printf("Number of threads: %d\n", num_threads);
        printf("Time used: %f seconds\n", time2 - time1);

        write_random_matrix_to_file(matrixA, matrixB, result, output_file);
        for (int i = 0; i < n; i++)
        {
            free(result[i]);
            free(matrixA[i]);
            free(matrixB[i]);
        }
        free(result);
        free(matrixA);
        free(matrixB);
    }
    else if (argc <= 3)
    {
        // Read file
        if (argc == 3)
        {
            input_file = argv[1];
            output_file = argv[2];
        }

        FILE *f = fopen(input_file, "r");
        if (!f)
        {
            printf("File could not be opened. Exiting.\n");
            exit(-1);
        }
        fscanf(f, "%d", &n);
        if (n % num_threads != 0)
        {
            printf("Number of threads should divide n.\n");
            fclose(f);
            exit(-1);
        }
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

        printf("Number of threads: %d\n", num_threads);
        printf("Time used: %f seconds\n", time2 - time1);

        write_matrix_to_file(result, output_file);
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
    }
}