# Add all targets (files with main fun ) to MAINS, even if unimportant!

CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Werror -O0 -std=c99 -g \
	$(shell pkg-config --cflags glib-2.0)
LDFLAGS = -lgc -lm
LDLIBS = $(shell pkg-config --libs glib-2.0)

MAINS = main_program _test _test_default_multi_bot _test_blobs _test_matrixflush _test_bb_flush nbtio_test route_test generate_default_trace run_trace make_plan txt2nbt nbt2txt build
MAIN_OBJS = $(patsubst %, %.o, $(MAINS))

SRCS = $(filter-out $(MAINS), $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
NONMAIN_OBJS = $(filter-out $(MAIN_OBJS), $(OBJS))
DEPS = $(OBJS:.o=.d)
FIXEDDEPS = $(DEPS:.d=.df)

SED = ./sed.sh

all: $(MAINS)

$(MAINS): $(NONMAIN_OBJS)

%.df: %.d
	$(SED) -r 's#/usr/[^[:space:]]+##g' < $< > $@

-include $(FIXEDDEPS)

clean:
	rm -rf *~ *.o *.d *.df *.dSYM $(MAINS)

.PHONY: all clean

