/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;


/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */
void handler(int sig) {
    fprintf(stdout, MESSAGE, num_reads, seconds);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    struct itimerval curr;
    getitimer(ITIMER_PROF, &curr);
    curr.it_value.tv_sec = seconds;
    setitimer(ITIMER_PROF, &curr, NULL);

    struct sigaction newact;
    newact.sa_handler = handler;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGPROF, &newact, NULL);

    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    int r;
    num_reads = 0;
    for (;;) {
        fseek(fp, sizeof(int) * (rand() % 100), SEEK_SET);

        fread(&r, 1, sizeof(int), fp);

        fprintf(stderr, "%d\n", r);
        num_reads += 1;


    }
    return 1; // something is wrong if we ever get here!
}
