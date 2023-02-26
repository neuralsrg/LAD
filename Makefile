all: output

output: main.o vector_generator.o
	g++ -g -Wall main.o vector_generator.o -o output

main.o: main.cpp
	g++ -g -Wall -c main.cpp

vector_generator.o: vector_generator.hpp vector_generator.cpp
	g++ -g -Wall -c vector_generator.cpp

clean:
	rm *.o output