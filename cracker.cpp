#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include <FL/fl_ask.H>
#include <FL/Fl_Multiline_Output.H>
#include "cracker.h"
#include "md5.h"
#include "pot.h"

extern bool addnl;
extern Fl_Multiline_Output *output;
extern Pot *pot;

bool Cracker::import_hashes() {
	FILE *fd = fopen(filename, "r");
	if(fd == NULL) {
		fl_message("Check your file's path");
		delete filename;
		return 1;
	}
	H = 0;
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
		tmphashlist[i][32] = 0;
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

	char *buff2 = new char[size];

	//We only do this to get the value a used to allocate arrays, and we'll repeat the procedure to fill these up.
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
		if(add)
		{
			hashlist[a] = new char[32];
			strcpy(hashlist[a], tmphashlist[i]);
			md5[a] = new unsigned int[4];
			for(int j=0; j < 4; j++) {
				char tmp2[9], tmp3[9];
				strcpy(tmp2, &hashlist[a][j*8]);
				tmp2[8] = 0;
				for(int k=0; k<8; k++) {
					tmp3[k] = tmp2[7-k];
				}
				tmp3[8] = 0;
				for(int k=0; k<8; k+=2) {
					char c = tmp3[k];
					tmp3[k] = tmp3[k+1];
					tmp3[k+1] = c;
				}
				md5[a][j] = strtol((char*)tmp3, NULL, 16);
			}
			a++;
		}
	}
	H = a;
	delete [] tmphashlist;
	return 0;
}

bool Cracker::hash_check(char *message) {
	unsigned int hash[4];
	md5_hash((unsigned char*) message, strlen(message), hash);	
	bool done = true;
	for(int h=0; h<H; ++h) {
		if(md5[h]) 
		{
			done = false;
			if(memcmp(hash, md5[h], sizeof(hash)) == 0) {
				//display related
				const char *previous = output->value();
				char tmp[strlen(previous)+strlen(message)+32];
				strcpy(tmp, previous);
				strcpy(&tmp[strlen(previous)], "\n");
				strcpy(&tmp[strlen(previous)+addnl], hashlist[h]);
				strcpy(&tmp[strlen(previous)+32+addnl], ":");
				strcpy(&tmp[strlen(previous)+33+addnl], message);
				//once one line has been written we always add a newline.
				addnl = true;
				int pos = output->position();
				output->value(tmp);
				output->position(pos);
				pot->save(hashlist[h], message);

				//delete
				md5[h] = NULL;
				delete md5[h];
			}
		}
	}
	return done;
}

Cracker::~Cracker() {
	if(crack) {
	 	delete [] hashlist;
		delete [] md5;
	}
}



