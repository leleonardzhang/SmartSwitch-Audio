#ifndef DECODER_GUARD
#define DECODER_GUARD

#ifndef NEURAL_NETWORK_PARAMS_GUARD
#include "neural_network_parameters.h"
#endif

#ifndef MATRIX_OPS_GUARD
#include "math/matrix_ops.h"
#endif

#ifndef FIXED_POINT_OPS_GUARD
#include "math/fixed_point_ops.h"
#endif

#ifndef UTILS_GUARD
#include "utils/utils.h"
#endif

#ifndef LAYERS_GUARD
#include "layers/layers.h"
#endif

#define DENSE_LAYER 0
#define LEAKY_RELU_LAYER 1
#define CONV2D_LAYER 2
#define MAXPOOLING2D_LAYER 3
#define FLATTEN_LAYER 4
#define DROPOUT_LAYER 5

#define LINEAR_ACTIVATION 0
#define SIGMOID_ACTIVATION 1
#define RELU_ACTIVATION 2

matrix *apply_model(matrix *output, matrix *input);

#endif
