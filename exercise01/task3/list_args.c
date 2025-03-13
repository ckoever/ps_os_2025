#include <stdio.h>
#include <stdlib.h>

// https://mywiki.wooledge.org/BashGuide/SpecialCharacters
int main(int argc, char* argv[]) {
	for (int i = 0; i < argc; ++i) {
		printf("%d: %s\n", i, argv[i]);
	}
	return EXIT_SUCCESS;
}
