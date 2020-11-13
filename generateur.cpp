#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
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
	Generateur::totCperlen = new uint_big [mmm+1];
	    
	Generateur::L = (int) strtol(strtok(NULL, ":"), NULL, 10);
	Generateur::A = (uint_big) strtoul(strtok(NULL, ":"), NULL, 10);
	
	for(int a=0; a<=mmm; ++a) {
	    Generateur::totCperlen[a] = (uint_big) strtoul(strtok(NULL, ":"), NULL, 10);
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
	fprintf(fd, "%d:", Generateur::length);
	fprintf(fd, "%s:", Generateur::arrayofchars);
	fprintf(fd, "%d:", Generateur::loop2);
	fprintf(fd, "%" PRIu64":", Generateur::a);
	for(int a=0; a<=Generateur::max-Generateur::min; ++a) {
	    fprintf(fd, "%" PRIu64":", Generateur::totCperlen[a]);
	    for(int b=0; b < Generateur::min+a; ++b)
			fprintf(fd, "%d:", Generateur::arrayofindex[a][b]); 
	}
	exit(0);
}

static uint_big rotate;

void Generateur::gen_next(int loop, char *tmp) {
	short int mpl = Generateur::min+loop;
 	
 	uint_big rotate = totCperlen[loop];
        
    tmp[0] = arrayofchars[arrayofindex[loop][0]];	
    
 	for(int i=1; i<mpl; ++i) {
    	tmp[i] = arrayofchars[(arrayofindex[loop][i]+rotate)%length];
        rotate /= (totCperlen[loop]) % (i/2+1) + 1;        
        
    }
    int pos = 0;
    totCperlen[loop]+=2;
	while(pos < mpl && ++arrayofindex[loop][pos] >= length) {
	    arrayofindex[loop][pos] = 0;
	    pos++;
    }
    tmp[mpl] = '\0';
}
