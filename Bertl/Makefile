
CFLAGS = -Wall -g -std=c99 -O2 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -MD
LFLAGS = -g

LDFLAGS = -lgc
LDLIBS = $(shell pkg-config --libs glib-2.0)


TARGETS = nbt2ascii ascii2nbt nbtio_test

all : $(TARGETS)

nbtio_test : nbtio.o nbtio_test.o


%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.nbt : %.txt
	./ascii2nbt >$@ <$<

clean :
	rm -f *~ *.o 
	rm -f $(TARGETS)

