# Add all targets (files with main fun ) to MAINS, even if unimportant!

CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Werror -O2 -std=c99 -g \
	$(shell pkg-config --cflags glib-2.0)
LDFLAGS = -lgc
LDLIBS = $(shell pkg-config --libs glib-2.0)

MAINS = _test _test_blobs nbtio_test generate_default_trace run_trace make_plan
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
	rm -f *~ *.o *.d *.df $(MAINS)

.PHONY: all clean

