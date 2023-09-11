#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pmake.h"


/* Read from the open file fp, and create the linked data structure
   that represents the Makefile contained in the file.
   See the top of pmake.h for the specification of Makefile contents.
 */
Rule *parse_file(FILE *fp) {
    // Implement this function and remove the stubbed return statement below.
	Rule *curr = malloc(sizeof(Rule));
	Rule *head = curr;

	curr -> target = NULL;
	curr -> dependencies = NULL;
	curr -> actions = NULL;
	curr -> next_rule = NULL;

	Action *ahead;
	Action *acurr;
	char c[MAXLINE];

	/*Here we initialize and allocate space for the rule and the action object
	 *and also initialize the reader
	 */	
	
	while(fgets(c, sizeof(c), fp) != NULL){
		if(is_comment_or_empty(c) == 1 || c[0] == '\n' || c[0] == '\r'
				|| c[0] == ' '){
			/*we will ingnore the line if the line is empty
			 */
			continue;
		}else if(c[0] != '\t'){
			/*when the line is not empty and not action,
			 *we run the following
			 */
			curr -> actions = ahead;

			/*we set the current actions to the action head
			 *that we have cumulated from the loop
			 */


			if(curr -> target != NULL){
				/*if it is not the head, we add a new rule
				 */
				curr -> next_rule = malloc(sizeof(Rule));
				curr = curr -> next_rule;
				curr -> target = NULL;
				curr -> dependencies = NULL;
				curr -> actions = NULL;
				curr -> next_rule = NULL;
			}

			char *token = strtok(c, " : ");
			curr -> target = malloc(strlen(token) + 1);
			strcpy(curr -> target, token);

			/*we copy the name of the rule to target
			 *
			 */

			acurr = malloc(sizeof(Action));
			acurr -> args = NULL;
			acurr -> next_act = NULL;
			ahead = acurr;

			/*initialize the head of the action
			 *
			 */

		}else if(c[0] == '\t'){
			if(acurr -> args != NULL){
				/*if we are not at the head
				 * we add a node of action
				 */
				acurr -> next_act = malloc(sizeof(Action));
				acurr = acurr -> next_act;
				acurr -> args = NULL;
				acurr -> next_act = NULL;			
			}


			char **args = malloc(sizeof(char *) * 20);

			//assume around 20 words

			char *token = strtok(c, " \r\n\t");
			int count = 0;

			while (token != NULL) {
    				args[count] = malloc(sizeof(char) * (strlen(token) + 1));
    				strcpy(args[count], token);
   				count += 1;
    				token = strtok(NULL, " \r\n\t");
			}

			for(int j = count; j < 20; j++){
				args[j] = NULL;
			}

			/*read all the actions into args and set the rest to be NULL
			 */			
			acurr -> args = args;
		}
	}
	curr -> actions = ahead;
	

	fseek(fp, SEEK_SET, 0);
	curr = head;

	/* return back to head
	 */

	while(fgets(c, sizeof(c), fp) != NULL){
		if(is_comment_or_empty(c) == 1 || c[0] == '\n' || c[0] == '\r'
				|| c[0] == ' '){
			continue;
		}
		Dependency *dhead;
		Dependency *dcurr;
			
		if(c[0] != '\t'){

			char *token = strtok(c, " \r\n\t");
			dhead = NULL;
			Rule* curr1;
			Dependency *dcurr1 = NULL;
			token = strtok(NULL, " \r\n\t");
			token = strtok(NULL, " \r\n\t");
			//goes to the start

			while(token != NULL ) {

				dcurr = malloc(sizeof(Dependency));
				dcurr -> rule = NULL;
				dcurr -> next_dep = NULL;
				curr1 = head;



				while(curr1 != NULL){
					if(strcmp(token, curr1 -> target) == 0){						
						dcurr -> rule = curr1;
						break;
					} 
					curr1 = curr1 -> next_rule;
				}

				if(dcurr -> rule == NULL){
					Rule *a = malloc(sizeof(Rule));
					a -> target = malloc(strlen(token) + 1);
					strcpy(a -> target, token);
					a -> dependencies = NULL;
					a -> actions = NULL;
					a -> next_rule = head;
					head = a;
					dcurr -> rule = a;

				}
				// if not found, we create a new rule at head


				if(dhead == NULL) {
       				    dhead = dcurr;
				    dcurr1 = dcurr;
                        
                                } else {
                                    dcurr1 -> next_dep = dcurr;
				    dcurr1 = dcurr1 -> next_dep; 
                                }
				
				token = strtok(NULL, " \r\n\t");
				
   			}
			if(curr != NULL){

				curr -> dependencies = dhead;
				curr = curr -> next_rule;
			}

		}

	
	}
	



    return head;
}



/******************************************************************************
 * These helper functions are provided for you. Do not modify them.
 *****************************************************************************/
/* Print the list of actions */
void print_actions(Action *act) {
    while(act != NULL) {
        if(act->args == NULL) {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while(act->args[i] != NULL) {
            printf("%s ", act->args[i]) ;
            i++;
        }
        printf("\n");
        act = act->next_act;
    }
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to
   run make correctly.
 */
void print_rules(Rule *rules){
    Rule *cur = rules;

    while (cur != NULL) {
        if (cur->dependencies || cur->actions) {
            // Print target
            printf("%s : ", cur->target);

            // Print dependencies
            Dependency *dep = cur->dependencies;
            while (dep != NULL){
                if(dep->rule->target == NULL) {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");

            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}


/* Return 1 if the line is a comment line, as defined on the assignment handout.
   Return 0 otherwise.
 */
int is_comment_or_empty(const char *line) {
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == '#') {
            return 1;
        }
        if (line[i] != '\t' && line[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

/* Convert an array of args to a single space-separated string in buffer.
   Returns buffer.  Note that memory for args and buffer should be allocted
   by the caller.
 */
char *args_to_string(char **args, char *buffer, int size) {
    buffer[0] = '\0';
    int i = 0;
    while (args[i] != NULL) {
        strncat(buffer, args[i], size - strlen(buffer));
        strncat(buffer, " ", size - strlen(buffer));
        i++;
    }
    return buffer;
}


