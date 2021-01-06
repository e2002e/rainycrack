#include <cstdint>
typedef unsigned __int128 uint_big;
class Generateur {
	public:
	int **arrayofindex;	//state
	char *arrayofchars;	//the character set
	uint_big a, A;		//counter
	int X, x;			//progressive mode related
	bool progressive;
	char *buff;			//holds the data when restoring
	int min, max;
	int m, M;
	int length;			//length of the character set
	int loop2, L;		//managing mutiple lengths
	bool gen_next(int, unsigned char*, int);
	void save();
	void restore();
	//for hash cracking
	bool crack;
	int type;
	int H;
	uint32_t **md5;
};
