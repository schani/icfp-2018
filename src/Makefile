# Add all targets (files with main fun ) to MAINS, even if unimportant!

CPPFLAGS = -MD
CC = gcc
CFLAGS = -Werror -O2 -std=c99 -g \
	$(shell pkg-config --cflags glib-2.0)
LDFLAGS = -lgc
LDLIBS = $(shell pkg-config --libs glib-2.0)

MAINS = _test nbtio_test generate_default_trace run_trace make_plan
MAIN_OBJS = $(patsubst %, %.o, $(MAINS))

SRCS = $(filter-out nbtio_test%, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: $(MAINS)

$(MAINS): $(filter-out $(MAIN_OBJS), $(OBJS))

clean:
	rm -f *~ *.o *.d $(MAINS)

.PHONY: all clean

-include $(DEPS)
