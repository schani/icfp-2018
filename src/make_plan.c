#include "plan.h"

int
main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s MODEL\n", argv[0]);
    }

    matrix_t model = read_model_file(argv[1]);
    matrix_t phases, blobs;

    make_plan(&model, &phases, &blobs);

    return 0;
}
