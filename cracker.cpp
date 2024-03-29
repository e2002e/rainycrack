#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include <FL/fl_ask.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include "cracker.h"
#include "pot.h"

extern bool addnl;
extern Fl_Text_Buffer *buff;
extern Fl_Text_Display *output;
extern Pot *pot;
bool addnl;

bool Cracker::import_hashes() {
	H = 0;
	FILE *fd = fopen(filename, "r");
	if(fd == NULL) {
		fl_message("Check your file's path");
		return 1;
	}
	do {
		int c = getc(fd);
		if(c == '\n') H++;
	} while(!feof(fd));
	fseek(fd, 0, SEEK_SET);

	type = 0;//TODO parse the type for sha1
	
	char **tmphashlist = new char *[H];
	for(int i=0; i<H; ++i) {
		tmphashlist[i] = new char[33];
		fread(tmphashlist[i], 33, 1, fd);
		tmphashlist[i][32] = '\0';
	}
	fclose(fd);

	FILE *found = fopen("rc.pot", "r");
	if(found == NULL) {
		found = fopen("rc.pot", "a+");
	}
	int size = 0;
	int h = 0;
	do {
		int c = getc(found);
		++size;
		if(c == '\n') h++;
	} while(!feof(found));
	fseek(found, 0, SEEK_SET);

	char *buff = new char[size];
	fread(buff, size, 1, found);
	fclose(found);
	int a = 0;
	//strtok modifies the source so we duplicate our hashes buffer
	char *buff2 = new char[size];

	//We only do this to get the value 'a' used to allocate arrays, and we'll repeat the procedure to fill these up.
	for(int i=0; i<H; i++) {
		bool add = true;
		strcpy(buff2, buff);
		for(int j=0; j<h; j++) {
			char pothash[33];
			if(j == 0) {
				strcpy(pothash, strtok(buff2, ":"));
				strtok(NULL, "\n");
			}
			else {
				strcpy(pothash, strtok(NULL, ":"));
				strtok(NULL, "\n");
			}
			if(strcmp(pothash, tmphashlist[i]) == 0)
				add = false;
		}
		if(add) {
			a++;
		}
	}
	if(a == 0) {
		fl_message("There is no reminding hash to crack in the list.");
		delete filename;
		delete [] tmphashlist;
		delete buff;
		delete buff2;
		return 1;	
	}
	hashlist = new char *[a];
	md5 = new unsigned int *[a];
	a = 0;
	
	for(int i=0; i<H; i++) {
		bool add = true;
		strcpy(buff2, buff);
		for(int j=0; j<h; j++) {
			char pothash[32];
			if(j == 0) {
				strcpy(pothash, strtok(buff2, ":"));
				strtok(NULL, "\n");
			}
			else {
				strcpy(pothash, strtok(NULL, ":"));
				strtok(NULL, "\n");
			}
			if(strcmp(pothash, tmphashlist[i]) == 0)
				add = false;
		}
		if(add) {
			hashlist[a] = new char[32];
			strcpy(hashlist[a], tmphashlist[i]);
			md5[a] = new unsigned int[4];
			for(int j=0; j < 4; j++) {
				char tmp2[9], tmp3[9];
				strcpy(tmp2, &hashlist[a][j*8]);
				tmp2[8] = 0;
				//little endian byteorder
				for(int k=0; k<8; k++) {
					tmp3[k] = tmp2[7-k];
				}

				tmp3[8] = 0;

				for(int k=0; k<8; k+=2) {
					char c = tmp3[k];
					tmp3[k] = tmp3[k+1];
					tmp3[k+1] = c;
				}
				md5[a][j] = strtoul((char*)tmp3, NULL, 16);
				//printf("%d", md5[a][j]);
			}
			//printf("\n");
			//printf("%s\n", hashlist[a]);
			a++;
			
		}
	}
	H = a;
	delete [] tmphashlist;
	return 0;
}
uint32_t hash[4];

bool Cracker::hash_check(char *message) {
	memset(hash, 0, sizeof(hash));
	md5_hash((unsigned char*) message, strlen(message), hash);	
	bool done = true;
	for(int h=0; h<H; ++h) {
		if(md5[h]) {
			done = false;
			if(memcmp(hash, md5[h], sizeof(hash)) == 0) {
				/*display related*/
				const char *previous = buff->text();
				char tmp[strlen(message) + 33];//pass + md5hash + nl;
				char *p = tmp;
				strcpy(p, hashlist[h]);
				p += strlen(hashlist[h]);
				strcpy(p, ":");
				p ++;
				strcpy(p, message);
				p += strlen(message);
				strcpy(p, "\n");
				output->insert(tmp);
				output->show_insert_position();
				pot->save(hashlist[h], message);
				md5[h] = NULL;
				//delete md5[h];
			}
		}
	}
	return done;
}

Cracker::~Cracker() {
	if(crack && H > 0) {
	 	delete [] hashlist;
		delete [] md5;
	}
}



