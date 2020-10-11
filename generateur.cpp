#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <omp.h>
#include <cinttypes>
#include "generateur.h"

void Generateur::restore() {
	Generateur::min = (int)strtol(strtok(Generateur::buff, ":"), NULL, 10);
	Generateur::max = (int)strtol(strtok(NULL, ":"), NULL, 10);
	Generateur::length = (int) strtol(strtok(NULL, ":"), NULL, 10);
 	Generateur::arrayofchars = new char[Generateur::length];
	strcpy(Generateur::arrayofchars, strtok(NULL, ":"));
	
	int mmm = Generateur::max-Generateur::min;
	
	Generateur::arrayofindex = new int *[mmm+1];
	Generateur::rotate = new uint_big [mmm+1];
	Generateur::totperlen = new uint_big [mmm+1];
	    
	Generateur::L = (int) strtol(strtok(NULL, ":"), NULL, 10);
	Generateur::A = (uint_big) strtoul(strtok(NULL, ":"), NULL, 10);
	
	for(int a=0; a<=mmm; ++a) {
	    Generateur::rotate[a] = (uint_big) strtoul(strtok(NULL, ":"), NULL, 10);
	    Generateur::totperlen[a] = (uint_big) strtoul(strtok(NULL, ":"), NULL, 10);
	    Generateur::arrayofindex[a] = new int[Generateur::max];
	 	for(int b=0; b<Generateur::min+a; ++b) {
 	    	Generateur::arrayofindex[a][b] = (int) strtol(strtok(NULL, ":"), NULL, 10);
		}
	}
}
//https://stackoverflow.com/questions/11656241/how-to-print-uint128-t-number-using-gcc
int fprintf_uint128(FILE *file, uint128_t n) {
  if (n == 0)  return printf("0\n");

  char str[40] = {0}; // log10(1 << 128) + '\0'
  char *s = str + sizeof(str) - 1; // start at the end
  while (n != 0) {
    if (s == str) return -1; // never happens

    *--s = "0123456789"[n % 10]; // save last digit
    n /= 10;                     // drop it
  }
  return fprintf(file, "%s:", s);
}

void Generateur::save() {
	FILE* fd = fopen("restore", "w");
	fprintf(fd, "%d:", Generateur::min);
	fprintf(fd, "%d:", Generateur::max);
	fprintf(fd, "%d:", Generateur::length);
	fprintf(fd, "%s:", Generateur::arrayofchars);
	fprintf(fd, "%d:", Generateur::loop2);
	fprintf_uint128(fd, Generateur::a);
	for(int a=0; a<=Generateur::max-Generateur::min; ++a) {
	    fprintf_uint128(fd, Generateur::rotate[a]);
	    fprintf_uint128(fd, Generateur::totperlen[a]);
	    for(int b=0; b < Generateur::min+a; ++b)
			fprintf(fd, "%d:", Generateur::arrayofindex[a][b]); 
	}
	exit(0);
}

void Generateur::gen_next(int loop, char *tmp) {
	short int mpl = Generateur::min+loop;
 	
 	tmp[0] = arrayofchars[arrayofindex[loop][0]];	
    for(int i=1; i<mpl; ++i) {
    	tmp[i] = arrayofchars[(arrayofindex[loop][i]+rotate[loop])%length];
    	rotate[loop] /= 2;
    }
    totperlen[loop]++;
    rotate[loop] = totperlen[loop];
    
	int pos = mpl - 1;
    while(pos >= 0 && ++arrayofindex[loop][mpl-pos-1] >= length) {
	    arrayofindex[loop][mpl-pos-1] = 0;
	    --pos;
    }
    tmp[mpl] = '\0';
}
