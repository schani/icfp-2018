#include "plan.h"

int
main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s MODEL\n", argv[0]);
    }

    matrix_t model = read_model_file(argv[1]);

    make_plan(&model);

    return 0;
}
