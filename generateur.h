#include <cstdint>

typedef unsigned __int128 uint_big;
class Generateur {
	public:
	int ***arrayofindex;//state
	void split_work();
	int min, max;
	int *loop, *L;

	char *arrayofchars;	//the character set
	int length;			//it's length

	//counters
	uint_big *a, *A;
	uint_big Counter;

	void gen_next(int, int, char *);

	uint_big **r;
	//save/restore
	void save();	
	bool restore();
};
