all: target.o

target.o: greedysolver.o basesolver.o
	g++ -std=c++17 -g -Wall -O3 greedysolver.o basesolver.o -o target.o

greedysolver.o: basesolver.o GreedySolver.hpp GreedySolver.cpp
	g++ -std=c++17 -g -Wall -O3 -c GreedySolver.cpp -o greedysolver.o

basesolver.o: BaseSolver.hpp BaseSolver.cpp
	g++ -std=c++17 -g -Wall -O3 -c BaseSolver.cpp -o basesolver.o



# some old useless

# all: output.o

output.o: parse_csv.o runc_bitset.o
	g++ -std=c++17 -g -Wall -O3 runc_bitset.o parse_csv.o -o output.o

parse_csv.o: ../csv/parse_csv.hpp ../csv/parse_csv.cpp
	g++ -std=c++17 -g -Wall -O3 -c ../csv/parse_csv.cpp -o parse_csv.o

runc_bitset.o: parse_csv.o runc_bitset.cpp
	g++ -std=c++17 -g -Wall -O3 -c runc_bitset.cpp -o runc_bitset.o

clean:
	rm *.o