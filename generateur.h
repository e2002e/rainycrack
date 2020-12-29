#include <cstdint>
typedef unsigned __int128 uint_big;
class Generateur {
	public:
	int **arrayofindex;
	char *arrayofchars;//the character set
	uint_big a;
	uint_big A;//counter
	int X, x;//progressive mode related
	bool progressive;
	char *buff;//holds the data when restoring
	int min, max;
	int length;//length of the character set
	int loop2, L;
	
	bool gen_next(int, char*, int);
	void save();
	void restore();
};
