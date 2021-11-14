#include "md5.h"
class Cracker {
	public:
	~Cracker();
	bool crack;
	unsigned int type;
	bool import_hashes();
	char *filename;
	bool hash_check(char *);
	private:
	int H;
	char **hashlist;
	unsigned int **md5;
};
