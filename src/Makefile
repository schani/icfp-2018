CPPFLAGS = -MD
CC = gcc
CFLAGS = -Werror -O2 -std=c99 -g \
	$(shell pkg-config --cflags glib-2.0)
LDFLAGS = -lgc
LDLIBS = $(shell pkg-config --libs glib-2.0)

BINS = _test generate_default_trace run_trace make_plan

SRCS = $(filter-out nbtio_test%, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: $(BINS)

# FIXME: generalize this again
_test: $(filter-out generate_default_trace% run_trace% make_plan%, $(OBJS)) 
generate_default_trace: $(filter-out _test% run_trace% make_plan%, $(OBJS))
run_trace: $(filter-out _test% generate_default_trace% make_plan%, $(OBJS))
make_plan: $(filter-out _test% generate_default_trace% run_trace%, $(OBJS))

clean:
	rm -f *~ *.o *.d $(BINS)

.PHONY: all clean

-include $(DEPS)
