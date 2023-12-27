all: output

output: parse_csv.o runc_bitset.o
	g++ -std=c++17 -g -Wall -O3 runc_bitset.o parse_csv.o -o output

parse_csv.o: ../csv/parse_csv.hpp ../csv/parse_csv.cpp
	g++ -std=c++17 -g -Wall -O3 -c ../csv/parse_csv.cpp -o parse_csv.o

runc_bitset.o: parse_csv.o runc_bitset.cpp
	g++ -std=c++17 -g -Wall -O3 -c runc_bitset.cpp -o runc_bitset.o

clean:
	rm *.o output