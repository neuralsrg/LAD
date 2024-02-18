all: target.o

target.o: main.o greedymodification.o greedysolver.o basesolver.o
	g++ -std=c++17 -g -Wall -O3 main.o greedymodification.o greedysolver.o basesolver.o -o target.o

main.o: main.cpp
	g++ -std=c++17 -g -Wall -O3 -c main.cpp -o main.o

greedymodification.o: greedysolver.o GreedyModification.hpp GreedyModification.cpp
	g++ -std=c++17 -g -Wall -O3 -c GreedyModification.cpp -o greedymodification.o

greedysolver.o: basesolver.o GreedySolver.hpp GreedySolver.cpp
	g++ -std=c++17 -g -Wall -O3 -c GreedySolver.cpp -o greedysolver.o

basesolver.o: BaseSolver.hpp BaseSolver.cpp
	g++ -std=c++17 -g -Wall -O3 -c BaseSolver.cpp -o basesolver.o

clean:
	rm *.o

rmlogs:
	rm results.txt
	rm time.txt


# old

# all: output.o

output.o: parse_csv.o runc_bitset.o
	g++ -std=c++17 -g -Wall -O3 runc_bitset.o parse_csv.o -o output.o

parse_csv.o: ../csv/parse_csv.hpp ../csv/parse_csv.cpp
	g++ -std=c++17 -g -Wall -O3 -c ../csv/parse_csv.cpp -o parse_csv.o

runc_bitset.o: parse_csv.o runc_bitset.cpp
	g++ -std=c++17 -g -Wall -O3 -c runc_bitset.cpp -o runc_bitset.o
