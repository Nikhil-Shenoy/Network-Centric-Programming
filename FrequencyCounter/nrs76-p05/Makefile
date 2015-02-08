CC=gcc
IDIR = include
CFLAGS=-I$(IDIR)

_DEPS = func.h
DEPS= $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR=obj
_OBJ = p05.o func.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

p05: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

debug: p05.c func.c include/func.h
	gcc -g p05.c func.c include/func.h
	gdb a.out

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm p05 
