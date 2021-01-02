#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdint.h>
#include "generateur.h"

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

static char *big2str(uint_big orig) {
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

static uint_big str2big(char *str) {
	int x;
	uint_big num = 0;
	int c = 0;
	for(x=strlen(str)-1; x>=0; --x) {
		num += (str[x]-'0') * powi(10, c);
		c++;
	}
	return num;
}

void Generateur::restore() {
	Generateur::min = (int)strtol(strtok(Generateur::buff, ":"), NULL, 10);
	Generateur::max = (int)strtol(strtok(NULL, ":"), NULL, 10);
 
 	int mmm = Generateur::max-Generateur::min;
	char *tmparrayofchars = strtok(NULL, ":");

	Generateur::length = strlen(tmparrayofchars);
	Generateur::arrayofchars = new char[Generateur::length];
	strcpy(Generateur::arrayofchars, tmparrayofchars);

	Generateur::progressive = (int) strtol(strtok(NULL, ":"), NULL, 10);
	Generateur::L = (int) strtol(strtok(NULL, ":"), NULL, 10);
	Generateur::X = (int) strtol(strtok(NULL, ":"), NULL, 10);
	
	char *tmpstr = strtok(NULL, ":");
	Generateur::A = str2big(tmpstr);
	
	Generateur::arrayofindex = new int *[mmm+1];
	for(int a=0; a<=mmm; ++a) {
	    Generateur::arrayofindex[a] = new int[Generateur::max];
	 	for(int b=0; b<Generateur::min+a; ++b) {
	 	    	Generateur::arrayofindex[a][b] = (int) strtol(strtok(NULL, ":"), NULL, 10);
		}
	}
}

void Generateur::save() {
	FILE* fd = fopen("restore", "w");
	fprintf(fd, "%d:", Generateur::min);
	fprintf(fd, "%d:", Generateur::max);
	fprintf(fd, "%s:", Generateur::arrayofchars);
	fprintf(fd, "%d:", Generateur::progressive);
	fprintf(fd, "%d:", Generateur::loop2);
	fprintf(fd, "%d:", Generateur::x);
	fprintf(fd, "%s:", big2str(Generateur::a));
	for(int a=0; a<=Generateur::max-Generateur::min; ++a) {
	    for(int b=0; b < Generateur::min+a; ++b)
			fprintf(fd, "%d:", Generateur::arrayofindex[a][b]); 
	}
	exit(0);
}

bool Generateur::gen_next(int loop, char *word, int step) {
	short unsigned int mpl = Generateur::min+loop, i;
	int over = 0;

	//TODO:find a way to replace a%step with a%length
	word[0] = arrayofchars[(arrayofindex[loop][0]+powi(step,mpl)/2+powi(step,mpl-1))%length];
	for(i=1; i<mpl; i++)
		word[i] = arrayofchars[(arrayofindex[loop][i]+length/(a%step+1)-1) % length];	

	for(i=0; i<mpl; i++) {
		if(++arrayofindex[loop][i] >= step) {
			arrayofindex[loop][i] = 0;
			over = 1;
			break;
		}
	}
	word[mpl] = '\0';
	return over;
}
