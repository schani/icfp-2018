#include <string.h>

#include "state.h"

state_t
make_state (void) {
    state_t s;
    memset(&s, 0, sizeof(state_t));
    return s;
}
