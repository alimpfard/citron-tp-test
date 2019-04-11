CFLAGS = -Wall -Werror -g

all: utf8proc libwordtree.so

utf8proc:
	make -C utf8

run: word-tree.o filter.o
	$(CC) $(CFLAGS) -o $@ word-tree.o filter.o utf8/utf8proc.o

libwordtree.so: CFLAGS += -fPIC # Build objects for .so with -fPIC.
libwordtree.so: libwordtree.a
	$(CC) -shared -o $@ -Wl,--whole-archive $^  -Wl,--no-whole-archive

libwordtree.a: word-tree.o filter.o
	ar cvq libwordtree.a utf8/utf8proc.o word-tree.o filter.o 

# Compile any .o from .c. Also make dependencies automatically.
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

#Include dependencies on subsequent builds.

.PHONY: all clean

clean:
	-rm -f *.o *.d run libwordtree.*
