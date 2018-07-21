#include <stdio.h>
#include <assert.h>

#include "model.h"
#include "execution.h"
#include "nbtio.h"

int
main (int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s MODEL TRACE\n", argv[0]);
        return 1;
    }

    matrix_t model = read_model_file(argv[1]);

    FILE *f = fopen(argv[2], "r");
    trace_t trace;
    int err = read_trace(f, &trace);
    assert(err == SUCCESS);
    fclose(f);

    matrix_t result = exec_trace(trace, model.resolution);
    assert(are_matrixes_equal(model, result));

    return 0;
}
