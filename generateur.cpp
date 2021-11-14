#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <FL/fl_ask.H>
#include "generateur.h"
#include "cracker.h"
#include "ui_options.h"

extern Cracker *cracker;
extern int mt;
extern bool method;
extern Ui_options *options;

//this function is from john/subsets.c
uint_big powi(uint32_t b, uint32_t p)
{
	uint_big res = 1;

	if (b == 0)
		return 0;

	while (p--) {
		uint_big temp = res * b;
		res = temp;
	}
	return res;
}

char *big2str(uint_big orig) {
	uint_big b = orig, total = 0;
	int c = 0;
	int x;
	static char str[41];
	do {
		str[c] = b%10 + '0';
		total += (b % 10) * powi(10, c);
		b /= 10;
		++c;
		
	} while(total < orig);
	
	char tmp[c/2];
	
	for(x=0; x<c; ++x) {
		if(x<c/2+c%2) {
			tmp[x] = str[x];
			str[x] = str[c-x-1];
		}
		else {
			str[x] = tmp[c-x-1];
		}
	}
	str[c] = '\0';
	return str;
}

uint_big str2big(char *str) {
	int x;
	uint_big num = 0;
	int c = 0;
	for(x=strlen(str)-1; x>=0; --x) {
		num += (str[x]-'0') * powi(10, c);
		c++;
	}
	return num;
}

Generateur::Generateur() {
	min = options->min;
	max = options->max;
	length = strlen(options->set);

	arrayofchars = new char[length];
	strcpy(arrayofchars, options->set);
	split_work();
}

Generateur::~Generateur() {
	delete [] arrayofindex;
	delete arrayofchars;
	delete tacke;
}
		
bool Generateur::restore() {
	FILE *fd = fopen("restore", "r");
	if(fd == NULL) {
		fl_message("Either you removed the restore file, or you did not interupt a single session.");
		return 1;
	}
	int filesize = 0;
	do {
		++filesize;
		getc(fd);
	} while(!feof(fd));//unroll the whole file to get the character's count. Yes C/C++ is ...
	char buff[filesize];
	fseek(fd, 0, SEEK_SET);
	fread(buff, filesize, 1, fd);
	fclose(fd);

	Generateur::min = (int)strtoul(strtok(buff, ":"), NULL, 10);
	Generateur::max = (int)strtoul(strtok(NULL, ":"), NULL, 10);
 
 	int mmm = Generateur::max-Generateur::min;
	char *tmparrayofchars = strtok(NULL, ":");	

	Generateur::length = strlen(tmparrayofchars);
	Generateur::arrayofchars = new char[Generateur::length];
	strcpy(Generateur::arrayofchars, tmparrayofchars);

	char *tmpstr = strtok(NULL, ":");
	Generateur::Counter = str2big(tmpstr);
	
	Generateur::arrayofindex = new int *[mmm+1];	
	tacke = new uint_big [mmm+1];
	Generateur::L = (int) strtoul(strtok(NULL, ":"), NULL, 10);
	char *tmpstr2 = strtok(NULL, ":");
	Generateur::A = str2big(tmpstr2);
	for(int x=0; x<=mmm; ++x) {
		Generateur::arrayofindex[x] = new int[Generateur::min+x];
	 	char *tmpstr3 = strtok(NULL, ":");
	 	tacke[a] = str2big(tmpstr3);
	 	for(int y=0; y<Generateur::min+x; ++y)
 	   		Generateur::arrayofindex[x][y] = (int) strtoul(strtok(NULL, ":"), NULL, 10);
	}
	cracker->crack = (int) strtoul(strtok(NULL, ":"), NULL, 10);
	method = (bool) strtoul(strtok(NULL, ":"), NULL, 10);
	if(cracker->crack) {
		char *tmpfilename = strtok(NULL, ":");
		cracker->filename = new char[strlen(tmpfilename)];
		strcpy(cracker->filename, tmpfilename);
		//printf(cracker->filename);
		if(cracker->import_hashes())
			return 1;
	}
	return 0;
}

void Generateur::split_work() {
	int mmm = max-min;
	L = 0;
	A = 0;
	tacke = new uint_big[mmm+1];
	arrayofindex = new int *[mmm+1];
	for(int x=0; x<=mmm; x++) {
		tacke[x] = 0;
		arrayofindex[x] = new int[min+x];
	    for(int y=0; y<min+x; ++y)
	        arrayofindex[x][y] = 0;
	}
	Counter = 0;
}

void Generateur::save() {
	FILE* fd = fopen("restore", "w");
	fprintf(fd, "%d:", Generateur::min);
	fprintf(fd, "%d:", Generateur::max);
	fprintf(fd, "%s:", Generateur::arrayofchars);
	fprintf(fd, "%s:", big2str(Generateur::Counter));
	
	fprintf(fd, "%d:", Generateur::loop);
	fprintf(fd, "%s:", big2str(Generateur::a));
	for(int a=0; a<=Generateur::max-Generateur::min; ++a) {
		fprintf(fd, "%s:", big2str(Generateur::tacke[a]));	
		for(int b=0; b < Generateur::min+a; ++b)
			fprintf(fd, "%d:", Generateur::arrayofindex[a][b]); 
	}
	fprintf(fd, "%d:", cracker->crack);
	fprintf(fd, "%d:", method);
	if(cracker->crack)
		fprintf(fd, "%s:", cracker->filename);
	fclose(fd);
}

void Generateur::gen_rain(int loop2, char *word) {
	short unsigned int mpl = min+loop2;
	for(int i=0; i<mpl; i++)
		word[i] = arrayofchars[arrayofindex[loop2][i]];
	word[mpl] = '\0';
	
	for(int x=0; x < mpl; ++x) {
		if(++arrayofindex[loop2][x] >= length) {
		    arrayofindex[loop2][x] = 0;
		    break;
		}
	}
}

void Generateur::gen_tacking(int loop2, char *word) {
	short unsigned int mpl = Generateur::min+loop2;
	for(int i=0; i<mpl; i++) {
		if(tacke[loop2] % 2 || mpl < 4) {
				word[i] = arrayofchars[arrayofindex[loop2][i]];
		}
		else {
				word[i] = arrayofchars[arrayofindex[loop2][mpl-i-1]];	
		}
	}
	if(tacke[loop2] % 2|| mpl < 4) {
		int pos = 0;
		while(pos < mpl && ++arrayofindex[loop2][pos] >= length) {
			arrayofindex[loop2][pos] = 0;
			pos++;
		}
	}
	else {
		for(int pos = mpl-1; pos >= 0; pos--) {
			if(mpl%2 == 0) {
				if(pos <= mpl / 2)
					break;
			}
			if(++arrayofindex[loop2][pos] >= length)
				arrayofindex[loop2][pos] = 0;
			else break;
		}
	}
	tacke[loop2]++;
}
