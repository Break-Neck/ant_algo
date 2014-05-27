test: build
	./test < input1.txt

build: test.o ant.o chooser.o
	c++ --std=c++11 test.o ant.o chooser.o -pthread -lboost_program_options -o test 

test.o:	test.cpp graph.hpp ant.hpp
	c++ --std=c++11 test.cpp -Wall -pthread -c 

ant.o: ant.cpp graph.hpp ant.hpp chooser.hpp
	c++ --std=c++11 ant.cpp -c -Wall 

chooser.o: chooser.cpp chooser.hpp
	c++ --std=c++11 chooser.cpp -c -Wall 

dejkstra: graph.hpp dejkstra.cpp
	c++ --std=c++11 dejkstra.cpp -Wall -o dejkstra 

clean:
	rm *.o dejkstra test ut_chooser
