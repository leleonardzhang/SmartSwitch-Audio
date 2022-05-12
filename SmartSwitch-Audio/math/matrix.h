#ifndef MATRIX_GUARD
#define MATRIX_GUARD

#ifndef _STDINT_H_
#include <stdint.h>
#endif

typedef int16_t dtype;

struct matrix {
    dtype *data;
    uint16_t numRows;
    uint16_t numCols;
};
typedef struct matrix matrix;

#endif
