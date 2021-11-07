main: main.o placement.o
	g++ main.o placement.o -o ../bin/main -O3
main.o: main.cpp placement.h
	g++ -c main.cpp -o main.o
placement.o: placement.cpp placement.h
	g++ -c placement.cpp -o placement.o
clean:
	rm -rf *.o ../bin/main