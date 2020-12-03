#include <cstdlib>
#include <cstdbool>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <signal.h>
#include "generateur.h"

bool check = 0;
bool restore = false;

extern uint_big powi(uint32_t b, uint32_t p);

void gen(Generateur *generateur) {
	for(generateur->loop2=generateur->L; generateur->loop2 <= generateur->max-generateur->min; ++generateur->loop2) {
		if(restore)
			generateur->save();

		uint_big subtotal = 0;

		if(generateur->loop2 > 0)
			subtotal = powi(generateur->length, generateur->min+generateur->loop2-1);

		for(generateur->a = generateur->A; generateur->a < \
				powi(generateur->length, generateur->min+generateur->loop2) - subtotal;
				++generateur->a) {
			if(restore)
				generateur->save();
			for(int loop = generateur->loop2; loop <= generateur->max-generateur->min; ++loop) {
				if(restore)
					generateur->save();
				char tmp[generateur->min+loop];
				generateur->gen_next(loop, tmp);
			    printf("%s\n", tmp);
			}
		}
	}
}
void showhelp() {
	printf(\
		"RainyCrack:\n\n"
		
		" Arguments:\n"
	  	"./rc [ min max --set | -set | -s \"set\" ] \n"
	  	"     [ -r | -restore | --restore ]\n\n"
	);
}

static void signalHandler(int sig) {
	restore = true;
}

int main(int argc, char *argv[]) {
	int arg;
	
	signal(SIGKILL,signalHandler);
	signal(SIGINT, signalHandler);
	
	Generateur *generateur = new Generateur;	
	if(argc > 2) {
	    if(!(generateur->min = atoi(argv[1]))) { fprintf(stderr, "Invalid minimum length: %d ...\n", generateur->min); exit(-1); }
		if(!(generateur->max = atoi(argv[2]))) { fprintf(stderr, "Invalid maximum length: %d ...\n", generateur->max); exit(-1); }
		
		for(arg=3; arg<argc; arg++) {
			if(strcmp(argv[arg], "--set") == 0 || strcmp(argv[arg], "-set") == 0 || strcmp(argv[arg], "-s") == 0) {
				generateur->length	= strlen(argv[arg+1]);
				generateur->arrayofchars = new char[generateur->length];
				strcpy(generateur->arrayofchars, argv[arg+1]);
				check = 1;
			}
			if(strcmp(argv[arg], "--restore") == 0 || strcmp(argv[arg], "-restore") == 0 || strcmp(argv[arg], "-r") == 0) {
				fprintf(stderr, "Use restore option as only parameter.\n");
				exit(-1);
			}
		}
		if(!check) { fprintf(stderr, "Needs input characters: --set option.\n"); exit(-1); }
	    else {
		    generateur->L = 0;
		    generateur->A = 0;
		    int mmm = generateur->max-generateur->min;
		    generateur->rain = new uint_big[mmm+1];
		    generateur->arrayofindex = new int *[mmm+1];
		    for(int a=0; a<=mmm; ++a) {
		    	generateur->rain[a] = 0;    
		    	generateur->arrayofindex[a] = new int [generateur->max];
		        for(int i=0; i<generateur->max; ++i)
		            generateur->arrayofindex[a][i] = 0;
		    }        
	    }
	}
	else if(argc == 2 && (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--restore") == 0 || strcmp(argv[1], "-restore") == 0)) {
		FILE *fd = fopen("restore", "r");
		int filesize = 0;
		do {
			++filesize;
			getc(fd);
		} while(!feof(fd));//unroll the whole file to get the character's count.
		generateur->buff = new char[filesize];
		fseek(fd, 0, SEEK_SET);
		fread(generateur->buff, filesize, 1, fd);
		fclose(fd);
		
		generateur->restore();		
	}
	else {
		showhelp();
		exit(0);
	}
	gen(generateur);
}

