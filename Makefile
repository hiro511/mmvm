TARGET = bin/mmvm
IDIR = ./mmvm2/include
CC=clang
CFLAGS=-I$(IDIR)
ODIR=obj
SRCDIR = mmvm2
SRCS = $(shell find  mmvm2  -maxdepth 1 -name "*.c")
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

mmvm: $(SRCS)
	mkdir -p bin
	$(CC) -o $(TARGET) $(CFLAGS) $(SRCS)

.PHONY: clean
clean:
	rm -f $(TARGET)
