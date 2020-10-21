CXX=g++

rc: main.o generateur.o
	$(CXX) main.o generateur.o -o rc
main.o: main.cpp
	$(CXX) -c main.cpp -o main.o
generateur.o: generateur.cpp generateur.h
	$(CXX) -c generateur.cpp -o generateur.o
clean:
	rm rc main.o generateur.o
