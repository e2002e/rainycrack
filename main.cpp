#include <cstdlib>
#include <cstdbool>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <signal.h>
#include <omp.h>
#include "generateur.h"
#include "md5.h"

bool check = 0;
bool stop = false;

extern uint_big powi(uint32_t b, uint32_t p);

void gen(Generateur *generateur) {
	int mmm = generateur->max-generateur->min;
	/*we are looping on the different lengths twice, once outside and once inside the main iteration for which we tweak
	the bound with subtotal, this is mandatory since we use the value of loop2 to calculate the size of the iteration.
	This this how we can work on all the lengths at the same time.*/
	for(generateur->x=generateur->X; generateur->x<=generateur->length*generateur->progressive; ++generateur->x) 
	{
		for(generateur->loop2=generateur->L; generateur->loop2 <= mmm; ++generateur->loop2) {
			int mpl = generateur->min+generateur->loop2;

			uint_big total;
			if(generateur->progressive)
				total = powi(generateur->x, mpl);
			else
				total = powi(generateur->length, mpl);

			uint_big subtotal = 0;
			if(generateur->loop2 > 0)
				if(generateur->progressive)
					subtotal = powi(generateur->x, mpl-1);
				else
					subtotal = powi(generateur->length, mpl-1);

			for(generateur->a = generateur->A; generateur->a < total - subtotal; ++generateur->a) {
				if(stop)
					generateur->save();
				//the inner loop on the lengths
				for(int loop = generateur->loop2; loop <= mmm; ++loop) {
					unsigned char tmp[generateur->min+loop];
					if(stop)
						generateur->save();
					if(generateur->progressive == 1) {
						if(generateur->gen_next(loop, tmp, generateur->x))
							printf("%s\n", tmp);
					}
					else {
						generateur->gen_next(loop, tmp, generateur->length);
						if(generateur->crack) {
							for(int h=0; h<generateur->H; ++h) {
								uint32_t hash[4];
								md5_hash(tmp, generateur->min+loop, hash);
								if(memcmp(hash, generateur->md5[h], sizeof(hash)) == 0) {
									printf("%s\n", tmp);
								}
							}
						}
						else
							printf("%s\n", tmp);
					}
				}
			}
		}
	}
}

void showhelp() {
	printf(\
		"RainyCrack:\n\n"
		" Password generator with deep mangling.\n"
		" Arguments:\n"
	  	"  ./rc [ min max --set | -set | -s \"set\" ] \n"
	  	"     [ -r | -restore | --restore ]\n"
		"     [ -p | -progressive | --progressive ]\n\n"
		
		" When using the -p option, mind the order of the character set. Hint:\n"
		" aeorisn1tl2md0cp3hbuk45g9687yfwjvzxqASERBTMLNPOIDCHGKFJUW.!Y*@V-ZQX\n _$#,/+?;^ %%~=&`\\)][:<(æ>\"ü|{'öä}\n\n"
	);
}

//this sets a global var tested in the gen() function => do we stop ?
static void signalHandler(int sig) {
	stop = true;
}

int main(int argc, char *argv[]) {
	int arg;
	
	signal(SIGKILL,signalHandler);
	signal(SIGINT, signalHandler);

	Generateur *generateur = new Generateur;	
	generateur->progressive = false;	
	generateur->crack = false;
	
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
			if(strcmp(argv[arg], "--progressive") == 0 || strcmp(argv[arg], "-progressive") == 0 || strcmp(argv[arg], "-p") == 0) {
				generateur->progressive = 1;
			}
			if(strcmp(argv[arg], "--crack") == 0 || strcmp(argv[arg], "-crack") == 0 || strcmp(argv[arg], "-c") == 0) {
				generateur->crack = true;
				if(strcmp(argv[arg+1], "md5") == 0)
					generateur->type = 0;
				else {
					fprintf(stderr, "Hash type unknown.\n");
					exit(-1);
				}
				FILE *fd = fopen(argv[arg+2], "r");
			 	if(fd == NULL)
			 	{
					fprintf(stderr, "Incorrect file path.\n");
					exit(-1);
				} 
				int filesize = 0;
				int h;
				do {
					++filesize;
					int c = getc(fd);
					if(c == '\n') generateur->H++;
				} while(!feof(fd));
				fseek(fd, 0, SEEK_SET);

				unsigned char **tmp = new unsigned char *[generateur->H];
				generateur->md5 = new uint32_t *[generateur->H];
				for(int i=0; i<generateur->H; ++i) {
					tmp[i] = new unsigned char[33];
					fread(tmp[i], 33, 1, fd);
					generateur->md5[i] = new uint32_t[4];
					for(int j=0; j < 4; j++) {
						char tmp2[8], tmp3[8];
						strncpy(tmp2, (char*)&tmp[i][j*8], 8);
						for(int k=0; k<8; ++k) {
							tmp3[k] = tmp2[7-k];
						}
						for(int l=0; l<8; l+=2) {
							char c = tmp3[l];
							tmp3[l] = tmp3[l+1];
							tmp3[l+1] = c;
						}
						generateur->md5[i][j] = strtol((char*)tmp3, NULL, 16);
					}
				}
				fclose(fd);
			}
		}
		if(!check) { fprintf(stderr, "Needs input characters: --set option.\n"); exit(-1); }
	    else {
		    generateur->L = 0;
		    generateur->A = 0;
		    generateur->X = 0;
		    int mmm = generateur->max-generateur->min;
		    generateur->arrayofindex = new int *[mmm+1];
		    for(int a=0; a<=mmm; ++a) {
				generateur->arrayofindex[a] = new int [generateur->max];
				for(int i=0; i<generateur->max; ++i)
		            generateur->arrayofindex[a][i] = 0;
		    }        
	    }
	}
	else if(argc == 2 && (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--restore") == 0 || strcmp(argv[1], "-restore") == 0)) {
		FILE *fd = fopen("restore", "r");
		if(fd == NULL)
		{
			fprintf(stderr, "Either you removed the restore file, or you did not interupt a session.\n");
			exit(-1);
		}
		int filesize = 0;
		do {
			++filesize;
			getc(fd);
		} while(!feof(fd));//unroll the whole file to get the character's count. Yes C/C++ is ...
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

