#ifndef MAT_H
#define MAT_H

#define PI 3.14159265358979323846
#define DEG2RAD(deg) ((deg)*PI / 180.0)

void transpose(float *dst, float *src, const int N, const int M);
void translate(float *T, float x, float y, float z);
void negate(float *T, float *A);
void rotate(float *T, float x, float y, float z);
void scale(float *T, float x, float y, float z);
void transform(float *L, float *T, float *R, float *S);
void transpose_inplace(float *src, const int N, const int M);
void print_mat(float *mat, const int m, const int n);

#ifdef MAT_IMPLEMENTATION
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "cblas.h"

// function to transpose a square matrix in place
void transpose_inplace(float *src, const int N, const int M)
{
    assert(N == M);
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < M; j++)
        {
            float temp = src[N * i + j];
            src[N * i + j] = src[N * j + i];
            src[N * j + i] = temp;
        }
    }
}

void transpose(float *dst, float *src, const int N, const int M)
{
    for (int n = 0; n < N * M; n++)
    {
        int i = n / N;
        int j = n % N;
        dst[n] = src[M * j + i];
    }
}

void translate(float *T, float x, float y, float z)
{
    float T1[16] = {
        1.0, 0.0, 0.0, x,
        0.0, 1.0, 0.0, y,
        0.0, 0.0, 1.0, z,
        0.0, 0.0, 0.0, 1.0}; // 4 X 4
    memcpy(T, T1, sizeof(float) * 16);
}

void negate(float *T, float *A)
{
    float N[16] = {
        -1.0, 0.0, 0.0, 0.0,
        0.0, -1.0, 0.0, 0.0,
        0.0, 0.0, -1.0, 0.0,
        0.0, 0.0, 0.0, 1.0}; // 4 X 4
    cblas_sgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        4, 4, 4,
        1.0, N, 4,
        A, 4,
        0.0, T, 4);
}

void rotate(float *T, float x, float y, float z)
{
    x = DEG2RAD(x);
    y = DEG2RAD(y);
    z = DEG2RAD(z);

    float temp[16] = {0.0};
    float cache_negative[6] = {0.0};
    float cache[6] = {
        cosf(z), sin(z),
        cosf(y), sin(y),
        cosf(x), sin(x)};

    negate(cache_negative, cache);

    float Z[16] = {
        cache[0], cache_negative[1], 0.0, 0.0,
        cache[1], cache[0], 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0}; // 4 X 4

    float Y[16] = {
        cache[2], 0.0, cache[3], 0.0,
        0.0, 1.0, 0.0, 0.0,
        cache_negative[3], 0.0, cache[2], 0.0,
        0.0, 0.0, 0.0, 1.0}; // 4 X 4

    float X[16] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, cache[4], cache_negative[5], 0.0,
        0.0, cache[5], cache[4], 0.0,
        0.0, 0.0, 0.0, 1.0}; // 4 X 4

    cblas_sgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        4, 4, 4,
        1.0, Z, 4,
        Y, 4,
        0.0, temp, 4);
    cblas_sgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        4, 4, 4,
        1.0, temp, 4,
        X, 4,
        0.0, T, 4);
}

void scale(float *T, float x, float y, float z)
{
    float T1[16] = {
        x, 0.0, 0.0, 0.0,
        0.0, y, 0.0, 0.0,
        0.0, 0.0, z, 0.0,
        0.0, 0.0, 0.0, 1.0}; // 4 X 4
    memcpy(T, T1, sizeof(float) * 16);
}

void transform(float *L, float *T, float *R, float *S)
{
    float temp[16] = {0.0};
    cblas_sgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        4, 4, 4,
        1.0, T, 4,
        R, 4,
        0.0, temp, 4);
    cblas_sgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        4, 4, 4,
        1.0, temp, 4,
        S, 4,
        0.0, L, 4);
}

void print_mat(float *mat, const int m, const int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
            printf("%3f\t", mat[i * n + j]);
        printf("\n");
    }
}

#endif
#endif
