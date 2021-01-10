#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include <FL/Fl_Output.H>
#include "cracker.h"
#include "md5.h"

extern bool addnl;
extern Fl_Output *output;

bool Cracker::import_hashes() {
	FILE *fd = fopen(filename, "r");
	if(fd == NULL) {
		delete filename;
		return 1;
	}
	int filesize = 0;
	H = 0;
	do {
		++filesize;
		int c = getc(fd);
		if(c == '\n') H++;
	} while(!feof(fd));
	fseek(fd, 0, SEEK_SET);

	type = 0;
	delete [] hashlist;
	hashlist = new char *[H];
	delete [] md5;
	md5 = new unsigned int *[H];
	for(int i=0; i<H; ++i) {
		hashlist[i] = new char[32];
		fread(hashlist[i], 33, 1, fd);
		md5[i] = new unsigned int[4];
		for(int j=0; j < 4; j++) {
			char tmp2[9], tmp3[9];
			strncpy(tmp2, (char*)&hashlist[i][j*8], 8);
			tmp2[8] = '\0';
			for(int k=0; k<8; ++k) {
				tmp3[k] = tmp2[7-k];
			}
			for(int l=0; l<8; l+=2) {
				char c = tmp3[l];
				tmp3[l] = tmp3[l+1];
				tmp3[l+1] = c;
			}
			tmp3[8] = '\0';
			md5[i][j] = strtoul((char*)tmp3, NULL, 16);
		}
	}
	fclose(fd);
	return 0;
}

bool Cracker::hash_check(char *message) {
	unsigned int hash[4];
	md5_hash((unsigned char*) message, strlen(message), hash);		
	bool done = true;
	for(int h=0; h<H; ++h) {
		if(md5[h]) {
			done = false;
			if(memcmp(hash, md5[h], sizeof(hash)) == 0) {
				const char *previous = output->value();
				char tmp[strlen(previous)+strlen(message)+32];
				strcpy(tmp, previous);
				strcpy(&tmp[strlen(previous)], "\n");
				strcpy(&tmp[strlen(previous)+addnl], hashlist[h]);
				strcpy(&tmp[strlen(previous)+32+addnl], ":");
				strcpy(&tmp[strlen(previous)+33+addnl], message);
				addnl = true;

				output->value(tmp);
				
				FILE *found = fopen("rc.pot", "a");
				fwrite((void *) hashlist[h], 32, 1, found);
				fwrite((void *) ":", 1, 1, found);
				fwrite((void *) message, strlen(message), 1, found);
				fwrite((void *) "\n", 1, 1, found);
				fclose(found);
				md5[h] = NULL;
			}
		}
	}
	return done;
}




