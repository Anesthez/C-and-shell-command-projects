#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: forkloop <iterations>\n");
        exit(1);
    }

    int iterations = strtol(argv[1], NULL, 10);
    int n = 1;
    int arr[iterations]; 

    for (int i = 0; i < iterations; i++) {
        if (n != 0) {
            n = fork();
	    arr[i] = n;
        }  else if(n < 0){
	    perror("fork");
	    exit(1);
	}
        printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
    }

    for (int i = 0; i < iterations; i++){
    	wait(&arr[i]);
    }

    return 0;
}
