#include <cstdint>

typedef unsigned long uint_big;
class Generateur {
	public:
	~Generateur();
	int ***arrayofindex;//state
	void split_work();
	int min, max;
	int *loop, *L;

	char *arrayofchars;	//the character set
	int length;			//it's length

	//counters
	uint_big *a, *A;
	uint_big Counter;

	void gen_tacking (int, int, char *);
	void gen_skyzo(int, int, char *);

	uint_big **tacke;	//bruteforcing on two sides is tacking
	//save/restore
	void save();	
	bool restore();
};
