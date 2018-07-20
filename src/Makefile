CPPFLAGS = -MD
CC = gcc
CFLAGS = -Werror -O2 -std=c99 -g
LDFLAGS =
LDLIBS =

BINS = _test

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: $(BINS)

$(BINS): $(OBJS)

clean:
	rm -f *~ *.o *.d $(BINS)

.PHONY: all clean

-include $(DEPS)
