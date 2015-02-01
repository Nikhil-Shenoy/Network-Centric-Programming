CC=gcc
IDIR = include
CFLAGS=-I$(IDIR)

_DEPS = func.h
DEPS= $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR=obj
_OBJ = counter.o func.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

counter: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

debug: counter.c
	gcc -g counter.c
	gdb a.out

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm counter
