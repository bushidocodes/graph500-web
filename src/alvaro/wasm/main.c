#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SIZE 2048

int **initializeMatrix(int size, int value)
{
    int i, j;

    int **matrix = malloc(size * sizeof *matrix);
    if (matrix == NULL)
    {
        fprintf(stderr, "initializeMatrix: malloc failed\n");
        return NULL;
    }
    for (i = 0; i < size; i++)
    {
        matrix[i] = malloc(size * sizeof *matrix[i]);
        if (matrix[i] == NULL)
        {
            fprintf(stderr, "initializeMatrix: row malloc failed\n");
            for (int k = 0; k < i; k++)
                free(matrix[k]);
            free(matrix);
            return NULL;
        }
    }

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            matrix[i][j] = value;
        }
    }

    return matrix;
}

int **initializeEmptyMatrix(int size)
{
    int i;

    int **matrix = malloc(size * sizeof *matrix);
    if (matrix == NULL)
    {
        fprintf(stderr, "initializeEmptyMatrix: malloc failed\n");
        return NULL;
    }
    for (i = 0; i < size; i++)
    {
        matrix[i] = malloc(size * sizeof *matrix[i]);
        if (matrix[i] == NULL)
        {
            fprintf(stderr, "initializeEmptyMatrix: row malloc failed\n");
            for (int k = 0; k < i; k++)
                free(matrix[k]);
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

/* Issue #12: helper to free a size x size matrix allocated by
 * initializeMatrix / initializeEmptyMatrix.
 * Protected by HAVE_FREE_MATRIX so the test file can pre-define it
 * (with counted free calls) without causing a duplicate symbol. */
#ifndef HAVE_FREE_MATRIX
void freeMatrix(int **m, int size)
{
    if (m == NULL)
        return;
    for (int i = 0; i < size; i++)
        free(m[i]);
    free(m);
}
#endif

void display(int **matrix, int size)
{
    int i, j;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int **add(int **m1, int **m2, int size)
{
    int i, j;
    int **mResult;
    mResult = initializeMatrix(size, 0);

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            mResult[i][j] = m1[i][j] + m2[i][j];
        }
    }
    return mResult;
}

int **multiply(int **m1, int **m2, int size)
{
    int i, j, k;
    int **mResult;
    mResult = initializeEmptyMatrix(size);

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            int sum = 0;
            for (k = 0; k < size; k++)
            {
                sum += m1[i][k] * m2[k][j];
            }
            mResult[i][j] = sum;
        }
    }
    return mResult;
}

int **getSubMatrix(int **matrix, int size, int xSection, int ySection)
{
    int i, j, fromY, fromX;
    int **subMatrix;
    subMatrix = initializeEmptyMatrix(size / 2);

    if (xSection == 1)
        fromX = 0;
    else
        fromX = size / 2;
    if (ySection == 1)
        fromY = 0;
    else
        fromY = size / 2;

    for (i = 0; i < size / 2; i++)
    {
        for (j = 0; j < size / 2; j++)
        {
            subMatrix[i][j] = matrix[i + fromY][j + fromX];
        }
    }

    return subMatrix;
}

int **getSuperMatrix(int **c11, int **c12, int **c21, int **c22, int size)
{
    int i, j;

    int **c;
    c = initializeEmptyMatrix(size * 2);

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            c[i][j] = c11[i][j];
            c[i][j + size] = c12[i][j];
            c[i + size][j] = c21[i][j];
            c[i + size][j + size] = c22[i][j];
        }
    }

    return c;
}

int **multiplyStrassens(int **a, int **b, int size)
{
    if (size < 256)
        return multiply(a, b, size);

    int half = size / 2;

    /* Partition A and B into quadrants */
    int **a11 = getSubMatrix(a, size, 1, 1);
    int **a12 = getSubMatrix(a, size, 1, 2);
    int **a21 = getSubMatrix(a, size, 2, 1);
    int **a22 = getSubMatrix(a, size, 2, 2);

    int **b11 = getSubMatrix(b, size, 1, 1);
    int **b12 = getSubMatrix(b, size, 1, 2);
    int **b21 = getSubMatrix(b, size, 2, 1);
    int **b22 = getSubMatrix(b, size, 2, 2);

    /* Compute result quadrants; free intermediate products immediately.
     * Issue #12: the original code used compound expressions like
     *   c11 = add(multiplyStrassens(...), multiplyStrassens(...), half)
     * which made both inner return values unreachable and permanently leaked. */

    int **prod1, **prod2;

    prod1 = multiplyStrassens(a11, b11, half);
    prod2 = multiplyStrassens(a12, b21, half);
    int **c11 = add(prod1, prod2, half);
    freeMatrix(prod1, half);
    freeMatrix(prod2, half);

    prod1 = multiplyStrassens(a11, b12, half);
    prod2 = multiplyStrassens(a12, b22, half);
    int **c12 = add(prod1, prod2, half);
    freeMatrix(prod1, half);
    freeMatrix(prod2, half);

    prod1 = multiplyStrassens(a21, b11, half);
    prod2 = multiplyStrassens(a22, b21, half);
    int **c21 = add(prod1, prod2, half);
    freeMatrix(prod1, half);
    freeMatrix(prod2, half);

    prod1 = multiplyStrassens(a21, b12, half);
    prod2 = multiplyStrassens(a22, b22, half);
    int **c22 = add(prod1, prod2, half);
    freeMatrix(prod1, half);
    freeMatrix(prod2, half);

    int **c = getSuperMatrix(c11, c12, c21, c22, half);

    /* Free all quadrant matrices now that getSuperMatrix has consumed them */
    freeMatrix(a11, half); freeMatrix(a12, half);
    freeMatrix(a21, half); freeMatrix(a22, half);
    freeMatrix(b11, half); freeMatrix(b12, half);
    freeMatrix(b21, half); freeMatrix(b22, half);
    freeMatrix(c11, half); freeMatrix(c12, half);
    freeMatrix(c21, half); freeMatrix(c22, half);

    return c;
}

void matrixMultiplication(int matrixSize)
{
    double totalTime;
    clock_t start, end;

    int **m1;
    int **m2;
    int **m3;

    m1 = initializeMatrix(matrixSize, 2);
    m2 = initializeMatrix(matrixSize, 3);
    if (m1 == NULL || m2 == NULL)
    {
        freeMatrix(m1, matrixSize);
        freeMatrix(m2, matrixSize);
        return;
    }

    printf("Multiplication started \n");
    start = clock();
    m3 = multiplyStrassens(m1, m2, matrixSize);
    end = clock();
    totalTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Strassens process time for %dx%d matrix multiplication: %f\n", matrixSize, matrixSize, totalTime);

    /* Issue #12: free all three matrices before returning */
    freeMatrix(m1, matrixSize);
    freeMatrix(m2, matrixSize);
    freeMatrix(m3, matrixSize);
    return;
}
