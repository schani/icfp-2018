#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "execution.h"
#include "model.h"
#include "nbtio.h"
#include "state.h"


int usage(int argc, char *argv[], char* error) {
    if (error!=NULL) {
        fprintf(stderr, "ERROR: %s\n", error);
    }
    fprintf(stderr, "Usage: %s TGTMODEL TRACE\n", argv[0]);
    fprintf(stderr, "     or \n");
    fprintf(stderr, "     : %s SRCMODEL TGTMODEL TRACE\n", argv[0]);
    fprintf(stderr, "Passing --- for a model assumes empty.");
    fprintf(stderr, "Calling in first form sets Lightning mode. The second Thunderbolt a.k.a. full mode.");
    return 1;
 }

int 
execute_trace_file(char* trace_file, matrix_t src, matrix_t tgt, task_mode_t mode) {
    FILE *f = fopen(trace_file, "r");
    trace_t trace;
    int err = read_trace(f, &trace);
    assert(err == SUCCESS);
    fclose(f);

    matrix_t result = exec_trace(trace, src, mode);

    assert(are_matrixes_equal(tgt, result));

    return 0;
}

static bool 
is_place_holder(char* filename) {
    return strcmp("---", filename) == 0;
}

int
main (int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        return usage(argc, argv, "wrong number of args");
    }

    char* tgt_file = "---";
    char* src_file = "---";
    char* trace_file = NULL;

    if (argc == 3) {
        tgt_file = argv[1];
        trace_file = argv[2];
    } else {  // argc == 4  
        src_file = argv[1];
        tgt_file = argv[2];
        trace_file = argv[3];
    }

    if (is_place_holder(tgt_file) && is_place_holder(src_file)) {
        return usage(argc, argv, "too many placeholders");
    }

    matrix_t tgt;
    matrix_t src;
    task_mode_t mode = (argc==3) ? Lightning : Thunderbolt;

    if (is_place_holder(src_file)) {
        tgt = read_model_file(tgt_file);
        src = make_matrix(tgt.resolution);
    } else if (is_place_holder(tgt_file)) {
        src = read_model_file(src_file);
        tgt = make_matrix(src.resolution);
    } else {
        src = read_model_file(src_file);
        tgt = read_model_file(tgt_file);
    }

    return execute_trace_file(trace_file, src, tgt, mode);
}
