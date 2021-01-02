#include <cstdint>
typedef unsigned __int128 uint_big;
class Generateur {
	public:
	int **arrayofindex;	//state
	char *arrayofchars;	//the character set
	uint_big a, A;		//counters for each roll
	uint_big counter;
	int X, x;			//progressive mode related
	bool progressive;
	char *buff;			//holds the data when restoring
	int min, max;
	int length;			//length of the character set
	int loop2, L;		//the loop on the words lengths, see main.cpp
	bool gen_next(int, char*, int);
	void save();
	void restore();
};
