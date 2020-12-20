#include <cstdint>
typedef unsigned __int128 uint_big;
class Generateur {
	public:
	int **arrayofindex;
	uint_big *rain;
	
	char *arrayofchars;//the character set
	uint_big a;
	uint_big A;//counter
	uint_big total;
	uint_big rotate;
	
	char *buff;//holds the data when restoring
	int min, max;
	int length;//length of the character set
	int loop2;
	int L;
	
	void gen_next(int, char*);
	void save();
	void restore();
};
