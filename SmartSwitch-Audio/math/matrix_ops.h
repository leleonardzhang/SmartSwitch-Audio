#ifndef MATRIX_OPS_GUARD
#define MATRIX_OPS_GUARD

#ifndef FIXED_POINT_OPS_GUARD
#include "fixed_point_ops.h"
#endif

#ifndef NEURAL_NETWORK_PARAMS_GUARD
#include "neural_network_parameters.h"
#endif


#ifdef IS_MSP
#ifndef __msp430
#include <msp430.h>
#endif

#ifndef __DSPLIB_H__
#include "DSPLib.h"
#endif

#define DMA_CHANNEL_0                                                    (0x00)
#define DMA_CHANNEL_1                                                    (0x10)
#define DMA_CHANNEL_2                                                    (0x20)
#define DMA_CHANNEL_3                                                    (0x30)
#define DMA_CHANNEL_4                                                    (0x40)
#define DMA_CHANNEL_5                                                    (0x50)
#define DMA_CHANNEL_6                                                    (0x60)
#define DMA_CHANNEL_7                                                    (0x70)
#endif

#define VECTOR_COLUMN(X)    ((X) * VECTOR_COLS)

//// For MSP implementations, we allocate memory in the LEA RAM.
//// This memory is used when executing matrix multiplications.
//DSPLIB_DATA(MULTIPLY_BUFFER, 4);
//static dtype MULTIPLY_BUFFER[1600];


matrix *filter_LEA(matrix* result, matrix *input, matrix *filter, uint16_t precision, uint16_t stride_numRows, uint16_t stride_numCols);
// Standard matrix operations
matrix *matrix_add(matrix *result, matrix *mat1, matrix *mat2);
matrix *matrix_multiply(matrix *result, matrix *mat1, matrix *mat2, uint16_t precision);
matrix *matrix_hadamard(matrix *result, matrix *mat1, matrix *mat2, uint16_t precision);
matrix *matrix_neg(matrix *result, matrix *mat, uint16_t precision);
matrix *scalar_product(matrix *result, matrix *mat, int16_t scalar, uint16_t precision);
matrix *scalar_add(matrix *result, matrix *mat, int16_t scalar);
matrix *apply_elementwise(matrix *result, matrix *mat, int16_t (*fn)(int16_t, uint16_t), uint16_t precision);
matrix *matrix_set(matrix *mat, int16_t value);
matrix *matrix_replace(matrix *dst, matrix *src);
matrix *vstack(matrix *result, matrix *mat1, matrix *mat2);
int16_t dot_product(matrix *vec1, matrix *vec2, uint16_t precision);
uint16_t *argsort(matrix *vec, uint16_t *result);
matrix *sparsemax(matrix *result, matrix *vec, uint16_t precision);

// Operations useful for various neural network functions
int16_t argmax(matrix *vec);
int16_t matrix_sum(matrix *mat);
int16_t matrix_min(matrix *mat);

int16_t *audio_fft(uint16_t *result, int16_t *data, uint16_t length);


static int16_t twiddleTable[258] = {
                                    0x0100, 0x0000,
                                        0x7FFF, 0x0000, 0x7FF6, 0xFCDC, 0x7FD9, 0xF9B8, 0x7FA7, 0xF695,
                                        0x7F62, 0xF374, 0x7F0A, 0xF055, 0x7E9D, 0xED38, 0x7E1E, 0xEA1E,
                                        0x7D8A, 0xE707, 0x7CE4, 0xE3F4, 0x7C2A, 0xE0E6, 0x7B5D, 0xDDDC,
                                        0x7A7D, 0xDAD8, 0x798A, 0xD7D9, 0x7885, 0xD4E1, 0x776C, 0xD1EF,
                                        0x7642, 0xCF04, 0x7505, 0xCC21, 0x73B6, 0xC946, 0x7255, 0xC673,
                                        0x70E3, 0xC3A9, 0x6F5F, 0xC0E9, 0x6DCA, 0xBE32, 0x6C24, 0xBB85,
                                        0x6A6E, 0xB8E3, 0x68A7, 0xB64C, 0x66D0, 0xB3C0, 0x64E9, 0xB140,
                                        0x62F2, 0xAECC, 0x60EC, 0xAC65, 0x5ED7, 0xAA0A, 0x5CB4, 0xA7BD,
                                        0x5A82, 0xA57E, 0x5843, 0xA34C, 0x55F6, 0xA129, 0x539B, 0x9F14,
                                        0x5134, 0x9D0E, 0x4EC0, 0x9B17, 0x4C40, 0x9930, 0x49B4, 0x9759,
                                        0x471D, 0x9592, 0x447B, 0x93DC, 0x41CE, 0x9236, 0x3F17, 0x90A1,
                                        0x3C57, 0x8F1D, 0x398D, 0x8DAB, 0x36BA, 0x8C4A, 0x33DF, 0x8AFB,
                                        0x30FC, 0x89BE, 0x2E11, 0x8894, 0x2B1F, 0x877B, 0x2827, 0x8676,
                                        0x2528, 0x8583, 0x2224, 0x84A3, 0x1F1A, 0x83D6, 0x1C0C, 0x831C,
                                        0x18F9, 0x8276, 0x15E2, 0x81E2, 0x12C8, 0x8163, 0x0FAB, 0x80F6,
                                        0x0C8C, 0x809E, 0x096B, 0x8059, 0x0648, 0x8027, 0x0324, 0x800A,
                                        0x0000, 0x8001, 0xFCDC, 0x800A, 0xF9B8, 0x8027, 0xF695, 0x8059,
                                        0xF374, 0x809E, 0xF055, 0x80F6, 0xED38, 0x8163, 0xEA1E, 0x81E2,
                                        0xE707, 0x8276, 0xE3F4, 0x831C, 0xE0E6, 0x83D6, 0xDDDC, 0x84A3,
                                        0xDAD8, 0x8583, 0xD7D9, 0x8676, 0xD4E1, 0x877B, 0xD1EF, 0x8894,
                                        0xCF04, 0x89BE, 0xCC21, 0x8AFB, 0xC946, 0x8C4A, 0xC673, 0x8DAB,
                                        0xC3A9, 0x8F1D, 0xC0E9, 0x90A1, 0xBE32, 0x9236, 0xBB85, 0x93DC,
                                        0xB8E3, 0x9592, 0xB64C, 0x9759, 0xB3C0, 0x9930, 0xB140, 0x9B17,
                                        0xAECC, 0x9D0E, 0xAC65, 0x9F14, 0xAA0A, 0xA129, 0xA7BD, 0xA34C,
                                        0xA57E, 0xA57E, 0xA34C, 0xA7BD, 0xA129, 0xAA0A, 0x9F14, 0xAC65,
                                        0x9D0E, 0xAECC, 0x9B17, 0xB140, 0x9930, 0xB3C0, 0x9759, 0xB64C,
                                        0x9592, 0xB8E3, 0x93DC, 0xBB85, 0x9236, 0xBE32, 0x90A1, 0xC0E9,
                                        0x8F1D, 0xC3A9, 0x8DAB, 0xC673, 0x8C4A, 0xC946, 0x8AFB, 0xCC21,
                                        0x89BE, 0xCF04, 0x8894, 0xD1EF, 0x877B, 0xD4E1, 0x8676, 0xD7D9,
                                        0x8583, 0xDAD8, 0x84A3, 0xDDDC, 0x83D6, 0xE0E6, 0x831C, 0xE3F4,
                                        0x8276, 0xE707, 0x81E2, 0xEA1E, 0x8163, 0xED38, 0x80F6, 0xF055,
                                        0x809E, 0xF374, 0x8059, 0xF695, 0x8027, 0xF9B8, 0x800A, 0xFCDC
};



static msp_cmplx_fft_q15_params fftParams;

#endif
