all: output

output: main.o vector_generator.o matrix.o
	g++ -g -Wall -O3 main.o vector_generator.o matrix.o -o output

main.o: main.cpp
	g++ -g -Wall -O3 -c main.cpp

vector_generator.o: vector_generator.hpp vector_generator.cpp
	g++ -g -Wall -O3 -c vector_generator.cpp

matrix.o: matrix.hpp matrix.cpp
	g++ -g -Wall -O3 -c matrix.cpp

clean:
	rm *.o output