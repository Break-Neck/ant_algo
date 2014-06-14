all: out/seller out/ant_way_finder

out/seller: out/seller.o out/ant.o out/chooser.o
	mkdir out -p
	c++ --std=c++11 out/seller.o out/ant.o out/chooser.o -pthread -lboost_program_options -o out/seller -O3 -DNDUBUG

out/ant_way_finder: out/ant_way_finder.o out/ant.o out/chooser.o 
	mkdir out -p
	c++ --std=c++11 out/ant_way_finder.o out/ant.o out/chooser.o -pthread -lboost_program_options -o out/ant_way_finder -O3 -DNDUBUG

out/ant_way_finder.o: ant_way_finder.cpp graph.hpp ant.hpp 
	mkdir out -p
	c++ --std=c++11 ant_way_finder.cpp -c -pthread -o out/ant_way_finder.o -O3 -DNDUBUG

out/seller.o: seller.cpp graph.hpp ant.hpp 
	mkdir out -p
	c++ --std=c++11 seller.cpp -pthread -c -o out/seller.o -O3 -DNDUBUG

out/ant.o: ant.cpp graph.hpp ant.hpp chooser.hpp 
	mkdir out -p
	c++ --std=c++11 ant.cpp -c -o out/ant.o -O3 -DNDUBUG

out/chooser.o: chooser.cpp chooser.hpp 
	mkdir out -p
	c++ --std=c++11 chooser.cpp -c -o out/chooser.o -O3 -DNDUBUG

dejkstra: out/dejkstra

out/dejkstra: graph.hpp dejkstra.cpp 
	mkdir out -p
	c++ --std=c++11 dejkstra.cpp -o out/dejkstra -O3 -DNDUBUG

clean: 
	rm out -r
