#include <stdio.h>

#define MAT_IMPLEMENTATION
#include "mat.h"

int main(void)
{
    printf("CBLAS Core: %s\n", OPENBLAS_CHAR_CORENAME);
    int m = 2;
    int k = 4;
    int n = 4;
    float A[8] = {
        1.0, 2.0, 3.0, 1.0,
        4.0, 5.0, 6.0, 1.0}; // 2 X 4

    float L[16] = {0.0};
    float T[16] = {0.0};
    float R[16] = {0.0};
    float S[16] = {0.0};

    float OUTPUT[16] = {0.0};

    translate(T, 0.0, 0.0, 0.0);
    rotate(R, 0.0, 0.0, 90.0);
    scale(S, 1.0, 1.0, 1.0);

    transform(L, T, R, S);
    transpose_inplace(L, k, n);
    cblas_sgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        m, n, k,
        1.0, A, k,
        L, n,
        0.0, OUTPUT, n);

    print_mat(A, m, k);
    printf("=======\n");
    print_mat(L, k, n);
    printf("=======\n");
    print_mat(OUTPUT, m, n);
    return 0;
}
