#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include "generateur.h"
#include "cracker.h"

extern Cracker *cracker;
extern int mt;

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

bool Generateur::restore() {
	FILE *fd = fopen("restore", "r");
	if(fd == NULL) {
		fprintf(stderr, "Either you removed the restore file, or you did not interupt a single session.\n");
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

	char *tmpstr2 = strtok(NULL, ":");
	Generateur::Counter = str2big(tmpstr2);

	arrayofindex = new int **[mt];
	L = new int [mt];
	A = new uint_big [mt];
	for(int t=0; t<mt; t++) {
		//starting value for the loops
		Generateur::L[t] = (int) strtoul(strtok(NULL, ":"), NULL, 10);
		char *tmpstr = strtok(NULL, ":");
		Generateur::A[t] = str2big(tmpstr);
		Generateur::arrayofindex[t] = new int *[mmm+1];	
		for(int a=0; a<=mmm; ++a) {
			Generateur::arrayofindex[t][a] = new int[Generateur::min+a];
		 	for(int b=0; b<Generateur::min+a; ++b)
	 	    	Generateur::arrayofindex[t][a][b] = (int) strtoul(strtok(NULL, ":"), NULL, 10);
		}
	}
	cracker->crack = (bool) strtoul(strtok(NULL, ":"), NULL, 10);
	if(cracker->crack) {
		char *tmpfilename = strtok(NULL, ":");
		cracker->filename = new char[strlen(tmpfilename)];
		strcpy(cracker->filename, tmpfilename);
	}
	return 0;
}

void Generateur::split_work() {
	int mmm = max-min;
	L = new int[mt];
	A = new uint_big[mt];
	arrayofindex = new int **[mt];
	for(int t=0; t<mt; t++){
		L[t] = 0;
		A[t] = 0;
		arrayofindex[t] = new int *[mmm+1];
		for(int a=0; a<=mmm; a++) {
			arrayofindex[t][a] = new int[min+a];
			uint_big step = powi(length, min+a) / mt * t;
			for(int b=0; b<min+a; b++) {
				arrayofindex[t][a][b] = step % length;
				step /= length;
			}
		}
	}
	Counter = 0;
}

void Generateur::save() {
	FILE* fd = fopen("restore", "w");
	fprintf(fd, "%d:", Generateur::min);
	fprintf(fd, "%d:", Generateur::max);
	fprintf(fd, "%s:", Generateur::arrayofchars);
	fprintf(fd, "%s:", big2str(Generateur::Counter));
	for(int t=0; t<mt; t++) {
		fprintf(fd, "%d:", Generateur::loop[t]);
		fprintf(fd, "%s:", big2str(Generateur::a[t]));
		for(int a=0; a<=Generateur::max-Generateur::min; ++a)
			for(int b=0; b < Generateur::min+a; ++b)
				fprintf(fd, "%d:", Generateur::arrayofindex[t][a][b]); 
	}
	fprintf(fd, "%d:", cracker->crack);
	if(cracker->crack)
		fprintf(fd, "%s:", cracker->filename);
	fclose(fd);
}

void Generateur::gen_next(int t, int loop2, char *word) {
	short unsigned int mpl = Generateur::min+loop2, i;
	uint_big r2 = r[t][loop2];
	r[t][loop2]+=length;
	word[0] = arrayofchars[arrayofindex[t][loop2][0]];
	for(i=1; i<mpl; i++) {
		word[i] = arrayofchars[(arrayofindex[t][loop2][i]+r2)%length];	
		r2 -= r2 / 2;
	}
	int pos = 0;
	while(pos < mpl && ++arrayofindex[t][loop2][pos] >= length) {
		arrayofindex[t][loop2][pos] = 0;
		pos ++;
	}
	word[mpl] = '\0';
}
