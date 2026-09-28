CFLAGS = -Wall -Wextra -g -std=gnu11 -pedantic

TQ = 4

all: scheduler test demo

debug: CFLAGS += -g
debug: clean all

scheduler: scheduler.o queue.o
	gcc -o scheduler scheduler.o queue.o
test: test.o queue.o
	gcc -o test test.o queue.o
demo: demo.o queue.o
	gcc -o demo demo.o queue.o
run: scheduler
	./scheduler $(TQ) processes.tsv


%.o: %.c queue.h
	gcc $(CFLAGS) -c $*.c

clean:
	rm -f *.o scheduler test demo


	
