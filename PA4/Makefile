CFLAGS = -Wall -Wextra -g -std=gnu11 -pedantic

debug: CFLAGS += -g
debug: clean all

memsimulator: memsimulator.c addresses.txt 
	gcc $(CFLAGS) -o memsimulator memsimulator.c

clean:
	rm -f *.o memsimulator


	
