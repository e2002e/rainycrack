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
	
	Generateur::arrayofindex = new int *[mmm+1];
	Generateur::rain = new uint_big [mmm+1];
	
	Generateur::L = (int) strtol(strtok(NULL, ":"), NULL, 10);
	char *tmpstr = strtok(NULL, ":");
	Generateur::A = str2big(tmpstr);
	
	for(int a=0; a<=mmm; ++a) {
		char *tmpstr2 = strtok(NULL, ":");
		Generateur::rain[a] = str2big(tmpstr2);
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
	fprintf(fd, "%d:", Generateur::loop2);
	fprintf(fd, "%s:", big2str(Generateur::a));
	for(int a=0; a<=Generateur::max-Generateur::min; ++a) {
	    fprintf(fd, "%s:", big2str(Generateur::rain[a]));
	    for(int b=0; b < Generateur::min+a; ++b)
			fprintf(fd, "%d:", Generateur::arrayofindex[a][b]); 
	}
	exit(0);
}

void Generateur::gen_next(int loop, char *tmp) {
	short int mpl = Generateur::min+loop;
	if(++rain[loop] >= length)
		rain[loop]=1;
	rotate = rain[loop];
	tmp[0] = arrayofchars[arrayofindex[loop][0]];

	for(int i=0; i<mpl; ++i) {
		tmp[i] = arrayofchars[(arrayofindex[loop][(i+rotate)%mpl])%length];
	}
	int pos = 0;
	while(pos < mpl && ++arrayofindex[loop][pos] >= length) {
	    arrayofindex[loop][pos]=0;
	    pos++;
    }
    tmp[mpl] = '\0';
}
