CXX=g++

rc: main.o generateur.o md5.o
	$(CXX) main.o generateur.o md5.o -o rc
main.o: main.cpp
	$(CXX) -c main.cpp -o main.o
generateur.o: generateur.cpp generateur.h
	$(CXX) -c generateur.cpp -o generateur.o -g
md5.o: md5.cpp md5.h
	$(CXX) -c md5.cpp -o md5.o

clean:
	rm rc main.o generateur.o
