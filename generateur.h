typedef unsigned __int128 uint_big;

class Generateur {
	public:
	int **arrayofindex;
	uint_big *rotate;
	uint_big *totperlen;
	
	char *arrayofchars;//the character set
	uint_big a;
	uint_big A;//counter
	
	char *buff;//holds the restore's data
	int min, max;
	int length;//length of the character set
	int loop2;
	int L;
	
	void gen_next(int, char *);
	void save();
	void restore();
};
