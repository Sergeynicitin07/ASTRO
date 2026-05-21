all: hh_sim

hh_sim: main.o struct.o methods.o
	gcc -o hh_sim main.o struct.o methods.o -lm

main.o: main.c struct.h methods.h
	gcc -c main.c

struct.o: struct.c struct.h
	gcc -c struct.c

methods.o: methods.c struct.h methods.h
	gcc -c methods.c

clean:
	rm -f *.o hh_sim
