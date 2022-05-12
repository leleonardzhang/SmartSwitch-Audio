#ifndef LAYERS_GUARD
#define LAYERS_GUARD

#ifndef _STDINT_H_
#include <stdint.h>
#endif

#ifndef MATRIX_OPS_GUARD
#include "math/matrix_ops.h"
#endif

#ifndef MATRIX_GUARD
#include "math/matrix.h"
#endif

#ifndef FIXED_POINT_OPS_GUARD
#include "math/fixed_point_ops.h"
#endif

#ifndef UTILS_GUARD
#include "utils/utils.h"
#endif

#ifndef NEURAL_NETWORK_PARAMS_GUARD
#include "neural_network_parameters.h"
#endif

// Standard Neural Network Functions

matrix *filter_simple(matrix *result, matrix *input, matrix *filter, uint16_t precision, uint16_t stride_numRows, uint16_t stride_numCols);
matrix *maxpooling(matrix* result, matrix *input, uint16_t pool_numRows, uint16_t pool_numCols);
matrix *flatten(matrix* result, matrix *input, uint16_t num_filter);
matrix *padding_same(matrix *result, matrix *input, matrix *filter, uint16_t stride_numRows, uint16_t stride_numCols);
matrix *maxpooling_filters(matrix *result, matrix *input, uint16_t numFilters, uint16_t pool_numRows, uint16_t pool_numCols);
matrix *filters_sum(matrix *result, matrix *input, matrix *filter, uint16_t numChannels, int16_t b, int16_t (*activation)(int16_t, uint16_t), uint16_t precision, uint16_t stride_numRows, uint16_t stride_numCols, uint16_t padding, uint16_t conv_numRows, uint16_t conv_numCols);
matrix *conv2d(matrix *result, matrix *input, matrix *filter, uint16_t numFilters, uint16_t numChannels, int16_t *b, int16_t (*activation)(int16_t, uint16_t), uint16_t precision, uint16_t stride_numRows, uint16_t stride_numCols, uint16_t padding);
matrix *apply_leakyrelu(matrix *result, matrix *input, uint16_t precision);
matrix *dense(matrix *result, matrix *input, matrix *W, matrix *b, int16_t (*activation)(int16_t, uint16_t), uint16_t precision);

#endif
