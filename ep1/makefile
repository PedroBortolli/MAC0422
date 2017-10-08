#
# Makefile para o ep1.
#

all: ep1 ep1sh tg

ep1: ep1.o
	$(CC) -pthread -o $@ $^ -lm

ep1sh: ep1sh.o
	$(CC) -o $@ $^ -lreadline

tg: tracegenerator.o
	$(CC) -o $@ $^

ep1.o: ep1.c ep1.h
	$(CC) -c $< -o $@

ep1sh.o: ep1sh.c ep1sh.h
	$(CC) -c $< -o $@

tracegenerator.o: tracegenerator.c
	$(CC) -c $< -o $@

clean:
	rm -rf *.o ep1 ep1sh tracegenerator
