class Cracker {
	public:
	bool crack;
	unsigned int type;
	bool import_hashes();
	char *filename;
	bool hash_check(char *);

	private:
	char **hashlist;
	int H;
	unsigned int **md5;
};
