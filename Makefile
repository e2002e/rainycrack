CXX=g++

rainycrack: main.o generateur.o cracker.o md5.o
	$(CXX) main.o generateur.o cracker.o md5.o -o rainycrack -lfltk -lpthread
main.o: main.cpp
	$(CXX) -c main.cpp -o main.o
generateur.o: generateur.cpp generateur.h
	$(CXX) -c generateur.cpp -o generateur.o
md5.o: md5.cpp md5.h
	$(CXX) -c md5.cpp -o md5.o
cracker.o: cracker.cpp cracker.h
	$(CXX) -c cracker.cpp -o cracker.o
clean:
	rm rainycrack main.o generateur.o cracker.o md5.o
