#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SIZE 2048

int **initializeMatrix(int size, int value)
{
    int **matrix;
    int i, j;

    matrix = malloc(size * sizeof *matrix);
    for (i = 0; i < size; i++)
    {
        matrix[i] = malloc(size * sizeof *matrix[i]);
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
    int **matrix;
    int i, j;

    matrix = malloc(size * sizeof *matrix);
    for (i = 0; i < size; i++)
    {
        matrix[i] = malloc(size * sizeof *matrix[i]);
    }
    return matrix;
}

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

    int i, j, k;

    int **a11;
    int **a12;
    int **a21;
    int **a22;

    a11 = getSubMatrix(a, size, 1, 1);
    a12 = getSubMatrix(a, size, 1, 2);
    a21 = getSubMatrix(a, size, 2, 1);
    a22 = getSubMatrix(a, size, 2, 2);

    int **b11;
    int **b12;
    int **b21;
    int **b22;

    b11 = getSubMatrix(b, size, 1, 1);
    b12 = getSubMatrix(b, size, 1, 2);
    b21 = getSubMatrix(b, size, 2, 1);
    b22 = getSubMatrix(b, size, 2, 2);

    int **c11;
    int **c12;
    int **c21;
    int **c22;

    c11 = add(multiplyStrassens(a11, b11, size / 2), multiplyStrassens(a12, b21, size / 2), size / 2);
    c12 = add(multiplyStrassens(a11, b12, size / 2), multiplyStrassens(a12, b22, size / 2), size / 2);
    c21 = add(multiplyStrassens(a21, b11, size / 2), multiplyStrassens(a22, b21, size / 2), size / 2);
    c22 = add(multiplyStrassens(a21, b12, size / 2), multiplyStrassens(a22, b22, size / 2), size / 2);

    int **c;

    c = getSuperMatrix(c11, c12, c21, c22, size / 2);

    return c;
}

void matrixMultiplication(int matrixSize)
{
    int y, x;
    double totalTime;
    clock_t start, end;

    int **m1;
    int **m2;
    int **m3;

    m1 = initializeMatrix(matrixSize, 2);
    m2 = initializeMatrix(matrixSize, 3);

    printf("Multiplication started \n");
    start = clock();
    m3 = multiplyStrassens(m1, m2, matrixSize);
    end = clock();
    totalTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Strassens process time for %dx%d matrix multiplication: %f\n", matrixSize, matrixSize, totalTime);
    return;
}

// int main(int argc, char *argv[])
// {

//     matrixMultiplication();

//     return 0;
// }
