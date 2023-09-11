#include <stdio.h>
#include <stdlib.h>

int populate_array(int, int *);
int check_sin(int *);


int main(int argc, char **argv) {
    // TODO: Verify that command line arguments are valid.

    // TODO: Parse arguments and then call the two helpers in sin_helpers.c
    // to verify the SIN given as a command line argument.
	if (!(argc == 2)) {
        return 2;
        }
	int sin = 0;
	sin = strtol(argv[1], NULL, 10);
	int sin_array[9];
	if(populate_array(sin, sin_array) == 1){
		printf("Invalid SIN\n");
		return 1;
	}
	if(check_sin(sin_array) == 1){
		printf("Invalid SIN\n");

		return 1;
	}
	printf("Valid SIN\n");
        return 0;
}
