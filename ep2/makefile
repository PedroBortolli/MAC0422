#
# Makefile para o ep1.
#

all: ep2

ep2: ep2.o barreira.o pista.o
	$(CC) -pthread -o $@ $^ -lm

ep2.o: ep2.c ep2.h
	$(CC) -c $< -o $@

barreira.o: barreira.c barreira.h
	$(CC) -c $< -o $@

pista.o: pista.c pista.h
	$(CC) -c $< -o $@

clean:
	rm -rf *.o ep2
