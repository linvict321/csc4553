CFLAGS = -Wall -Wextra -g -std=gnu11 -pedantic

TQ = 4

all: scheduler test demo

debug = CFLAGS += -g
debug: clean all

scheduler: scheduler.o queue.o
	gcc -o scheduler scheduler.o
test: test.o
	gcc -o test test.o queue.o
demo: demo.o
	gcc -o demo demo.c queue.o
run: scheduler
	./scheduler $(TQ) processes.tsv


%.o: %.c
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o scheduler test demo


	