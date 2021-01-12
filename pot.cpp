#include <cstdio>
#include <cstring>
#include "pot.h"

void Pot::save(char *hash, char *message) {
		FILE *found = fopen("rc.pot", "a");
		fwrite((void *) hash, 32, 1, found);
		fwrite((void *) ":", 1, 1, found);
		fwrite((void *) message, strlen(message), 1, found);
		fwrite((void *) "\n", 1, 1, found);
		fclose(found);
}
