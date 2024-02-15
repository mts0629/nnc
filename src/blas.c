/**
 * @file blas.c
 * @brief Matrix operations for data array
 *
 */
#include <stddef.h>
#include <math.h>

#include "blas.h"

void scopy(const int n, const float *x, const int incx, float *y, const int incy) {
    if ((x == NULL) || (y == NULL)) {
        return;
    }
    if ((n < 1) || (incx == 0) || (incy == 0)) {
        return;
    }

    int x_idx = (incx > 0) ? 0 : (n * -incx - 1);
    int y_idx = (incy > 0) ? 0 : (n * -incy - 1);

    for (int i = 0; i < n; i++) {
        y[y_idx] = x[x_idx];
        x_idx += incx;
        y_idx += incy;
    }
}

float sdot(const int n, const float *x, const int incx, const float *y, const int incy) {
    if ((x == NULL) || (y == NULL)) {
        return 0;
    }
    if ((n < 1) || (incx == 0) || (incy == 0)) {
        return 0;
    }

    float dot = 0;
    if ((incx == 1) && (incy == 1)) {
        for (int i = 0; i < n; i++) {
            dot += x[i] * y[i];
        }
    } else {
        // if incx/incy < 0, working backward
        int x_idx = (incx > 0) ? 0 : (n * -incx - 1);
        int y_idx = (incy > 0) ? 0 : (n * -incy - 1);
        for (int i = 0; i < n; i++) {
            dot += x[x_idx] * y[y_idx];
            x_idx += incx;
            y_idx += incy;
        }
    }

    return dot;
}

float snrm2(const int n, const float *x, const int incx) {
    if (x == NULL) {
        return 0;
    }
    if ((n < 1) || (incx == 0)) {
        return 0;
    }

    float nrm2 = 0;
    if (incx == 1) {
        for (int i = 0; i < n; i++) {
            nrm2 += x[i] * x[i];
        }
    } else {
        // if incx < 0, working backward
        int x_idx = (incx > 0) ? 0 : (n * -incx - 1);
        for (int i = 0; i < n; i++) {
            nrm2 += x[x_idx] * x[x_idx];
            x_idx += incx;
        }
    }

    return sqrtf(nrm2);
}

void saxpy(const int n, const float alpha, const float *x, const int incx, float *y, const int incy) {
    if ((x == NULL) || (y == NULL)) {
        return;
    }
    if ((n < 1) || (incx == 0) || (incy == 0)) {
        return;
    }

    if ((incx == 1) && (incy == 1)) {
        for (int i = 0; i < n; i++) {
            y[i] += alpha * x[i];
        }
    } else {
        // if incx/incy < 0, working backward
        int x_idx = (incx > 0) ? 0 : (n * -incx - 1);
        int y_idx = (incy > 0) ? 0 : (n * -incy - 1);
        for (int i = 0; i < n; i++) {
            y[y_idx] += alpha * x[x_idx];
            x_idx += incx;
            y_idx += incy;
        }
    }
}

void sgemv(
    const BlasTranspose trans,
    const int M, const int N,
    const float alpha, const float *A, const int lda,
    const float *x, const int incx,
    const float beta, float *y, const int incy) {
    if ((A == NULL) || (x == NULL) || (y == NULL)) {
        return;
    }
    if ((M < 1) || (N < 1) || (incx == 0) || (incy == 0)) {
        return;
    }
    if (((trans == BLAS_NO_TRANS) && (lda < N)) ||
        ((trans == BLAS_TRANS) && (lda < M))) {
        return;
    }

    if (trans == BLAS_NO_TRANS) {
        // a: NO_TRANS
        int y_idx = (incy > 0) ? 0 : (M * -incy - 1);
        for (int i = 0; i < M; i++) {
            float sum = 0;
            int x_idx = (incx > 0) ? 0 : (N * -incx - 1);
            for (int j = 0; j < N; j++) {
                sum += alpha * A[i * lda + j] * x[x_idx];
                x_idx += incx;
            }
            y[y_idx] *= beta;
            y[y_idx] += sum;
            y_idx += incy;
        }
    } else {
        // a: TRANS
        int y_idx = (incy > 0) ? 0 : (M * -incy - 1);
        for (int i = 0; i < M; i++) {
            float sum = 0;
            int x_idx = (incx > 0) ? 0 : (N * -incx - 1);
            for (int j = 0; j < N; j++) {
                sum += alpha * A[j * lda + i] * x[x_idx];
                x_idx += incx;
            }
            y[y_idx] *= beta;
            y[y_idx] += sum;
            y_idx += incy;
        }
    }
}

void sgemm(
    const BlasTranspose transa, const BlasTranspose transb,
    const int M, const int N, const int K,
    const float alpha, const float *A, const int lda,
    const float *B, const int ldb,
    const float beta, float *C, const int ldc) {
    if ((A == NULL) || (B == NULL) || (C == NULL)) {
        return;
    }
    if ((M < 1) || (N < 1) || (K < 1)) {
        return;
    }
    if (((transa == BLAS_NO_TRANS) && (lda < K)) ||
        ((transa == BLAS_TRANS) && (lda < M))) {
        return;
    }
    if (((transb == BLAS_NO_TRANS) && (ldb < N)) ||
        ((transb == BLAS_TRANS) && (ldb < K))) {
        return;
    }
    if (ldc < N){
        return;
    }

    if (transa == BLAS_NO_TRANS) {
        if (transb == BLAS_NO_TRANS) {
            // a: NO_TRANS, b: NO_TRANS
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    float sum = 0;
                    for (int k = 0; k < K; k++) {
                        sum += A[i * lda + k] * B[k * ldb + j];
                    }
                    C[i * ldc + j] *= beta;
                    C[i * ldc + j] += alpha * sum;
                }
            }
        } else {
            // a: NO_TRANS, b: TRANS
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    float sum = 0;
                    for (int k = 0; k < K; k++) {
                        sum += A[i * lda + k] * B[j * ldb + k];
                    }
                    C[i * ldc + j] *= beta;
                    C[i * ldc + j] += alpha * sum;
                }
            }
        }
    } else {
        if (transb == BLAS_NO_TRANS) {
            // a: TRANS, b: NO_TRANS
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    float sum = 0;
                    for (int k = 0; k < K; k++) {
                        sum += A[k * lda + i] * B[k * ldb + j];
                    }
                    C[i * ldc + j] *= beta;
                    C[i * ldc + j] += alpha * sum;
                }
            }
        } else {
            // a: TRANS, b: TRANS
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    float sum = 0;
                    for (int k = 0; k < K; k++) {
                        sum += A[k * lda + i] * B[j * ldb + k];
                    }
                    C[i * ldc + j] *= beta;
                    C[i * ldc + j] += alpha * sum;
                }
            }
        }
    }
}