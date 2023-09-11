#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

void Close(int fd){
	if(close(fd) < 0){
		perror("close");
		exit(1);
	}
}

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
     Please read the comments in validate carefully
   */

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }

  if(strlen(user_id) > 10){
      fprintf(stdout, NO_USER);
      return 0;
  }

  if(strlen(password) > 10){
      fprintf(stdout, INVALID);
      return 0;
  }


  int fd[2];
  int n;
  
  if(pipe(fd) < 0){
  	perror("pipe");
	exit(1);
  }

  
  int result = fork();
 
  if(result < 0){
  	perror("fork");
	exit(1);
  }else if(result == 0){
	Close(fd[1]);
	if (dup2(fd[0], fileno(stdin)) == -1) {
		perror("dup2");
	}
	
	execl("./validate", user_id, password, NULL);
	Close(fd[0]);
	perror("exec");
	exit(1);
  	
  }else{
	int status;
	Close(fd[0]);
	if((n = write(fd[1], user_id, 10)) == -1) {
        perror("write");
        exit(1);
        }

	
	if((n = write(fd[1], password, 10)) == -1) {
        perror("write");
        exit(1);
        }


	if (wait(&status) != -1 && WIFEXITED(status)) {
		if (WEXITSTATUS(status) == 3) {
	            fprintf(stdout, NO_USER);
		} else if (WEXITSTATUS(status) == 2) {
	            fprintf(stdout, INVALID);
		} else if (WEXITSTATUS(status) == 1){
		    exit(1);
		} else {
	            fprintf(stdout, SUCCESS);
		}
	
	}

	Close(fd[1]);
  }


  return 0;
}


