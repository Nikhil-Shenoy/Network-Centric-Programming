CC=gcc
#CFLAGS=-I
#DEPS=

%.o: %.c 
	$(CC) -c -o $@ $<

counter: counter.o
	gcc -o counter counter.o

debug: counter.c
	gcc -g counter.c
	gdb a.out

clean:
	rm counter
	rm *.o
	rm a.out
