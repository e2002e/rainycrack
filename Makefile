CXX=g++

rainycrack: main.o generateur.o cracker.cpp md5.o pot.o
	$(CXX) main.o generateur.o cracker.cpp md5.cpp pot.o -o rainycrack -lfltk -lpthread -fpermissive
main.o: main.cpp
	$(CXX) -c main.cpp -o main.o -g
generateur.o: generateur.cpp generateur.h
	$(CXX) -c generateur.cpp -o generateur.o -g
pot.o: pot.cpp pot.h
	$(CXX) -c pot.cpp -o pot.o
clean:
	rm rainycrack main.o generateur.o cracker.o md5.o pot.o
