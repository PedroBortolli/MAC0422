#
# Makefile para o ep3.
#

all: ep3

ep3: ep3.o aloc.o page.o simulate.o files.o
	$(CXX) -o $@ $^ -lreadline

ep3.o: ep3.cpp ep3.h
	$(CXX) -c $< -o $@

aloc.o: aloc.cpp aloc.h
	$(CXX) -c $< -o $@

page.o: page.cpp page.h
	$(CXX) -c $< -o $@

simulate.o : simulate.cpp simulate.h
	$(CXX) -c $< -o $@

files.o : files.cpp files.h
	$(CXX) -c $< -o $@

clean:
	rm -rf *.o ep3
