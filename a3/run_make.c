#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"


/* Evaluate the rule in rules corresponding to the given target.
   If target is NULL, evaluate the first rule instead.
   If pflag is 0, evaluate each dependency in sequence.
   If pflag is 1, then evaluate each dependency in parallel (by creating one 
   new process per dependency). In this case, the parent process will wait until
   all child processes have terminated before checking dependency modified times
   to decide whether to execute the actions.
 */


void run_rule(Rule *rules){
	// helper method to run a rule
	Action *act = rules -> actions;
	while(act != NULL){
		int r = fork();
		if(r < 0){
			perror("fork");
			exit(1);
		}else if(r == 0){
			int status = execvp(act -> args[0],act -> args);

			if(status < 0){
				perror("exe");
				exit(1);
			}

			exit(0);
		}else{
			if(wait(&r) != -1){
				if(WEXITSTATUS(r) != 0){
					exit(1);
				}	
			}

			int i = 0;
        		while(act->args[i] != NULL) {
            		printf("%s ", act->args[i]) ;
            		i++;
       	 		}
			printf("\n");

		}

		act = act -> next_act;
	}
}



int checktime(char* target, char* dep){

	struct stat timetarget;
	struct stat timedep;


	stat(target, &timetarget);
	stat(dep, &timedep);

	if(timetarget.st_mtimespec.tv_sec < timedep.st_mtimespec.tv_sec || 
	   (timetarget.st_mtimespec.tv_sec = timedep.st_mtimespec.tv_sec && 
	    timetarget.st_mtimespec.tv_nsec < timedep.st_mtimespec.tv_nsec)){
		return 0;
	}

	return 1;
}


void evaluate1(Rule *rules){
	int count = 0;
	int status;

	if(rules -> dependencies == NULL){
		if(rules -> actions != NULL){
			run_rule(rules);
		}
	}else{
		Dependency *dcurr;
		dcurr = rules -> dependencies;
		while(dcurr != NULL){
			int r = fork();

			if(r < 0){
				perror("fork");
				exit(1);
			}else if(r == 0){
				evaluate1(dcurr -> rule);
				exit(0);
			}
			count += 1;
			dcurr = dcurr -> next_dep;
		}

		while(count > 0){
			if(wait(&status) != -1){
				if(WEXITSTATUS(status) != 0){
				exit(1);
				}	
			}
			count = count - 1;

		}

		struct stat target;

		if(stat(rules -> target, &target) == -1){
			run_rule(rules);
		}else{
			Dependency *dcurr;
			dcurr = rules -> dependencies;
			while(dcurr != NULL){
				if(checktime(rules -> target, 
				     	     dcurr -> rule -> target) == 0){
					run_rule(rules);
				}
				dcurr = dcurr -> next_dep;
			}

		}
	}
}

void evaluate0(Rule *rules){
	if(rules -> dependencies == NULL){
		if(rules -> actions != NULL){
			run_rule(rules);
		}
	}else{
		Dependency *dcurr;
		dcurr = rules -> dependencies;
		while(dcurr != NULL){
			evaluate0(dcurr -> rule);
			dcurr = dcurr -> next_dep;
		}
		struct stat target;

		if(stat(rules -> target, &target) == -1){
			run_rule(rules);
		}else{
			Dependency *dcurr;
			dcurr = rules -> dependencies;
			while(dcurr != NULL){
				if(checktime(rules -> target, 
				     	     dcurr -> rule -> target) == 0){
					run_rule(rules);
				}
				dcurr = dcurr -> next_dep;
			}

		}
	}

	
}




void run_make(char *target, Rule *rules, int pflag) {
	if(target == NULL){
		if(pflag == 0){
			evaluate0(rules);
		}else{
			evaluate1(rules);
		}
	}else{
		Rule* curr = rules;
		while(curr != NULL){
			if(strcmp(target, curr -> target) == 0){
				break;
			}
			curr = curr -> next_rule;
		}
		if(curr == NULL){
			printf("target not found");
		}

		if(pflag == 0){
			evaluate0(curr);
		}else{
			evaluate1(curr);
		}
	}
	
}

