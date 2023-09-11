#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pmake.h"


/* Read from the open file fp, and create the linked data structure
   that represents the Makefile contained in the file.
   See the top of pmake.h for the specification of Makefile contents.
 */
Rule *parse_file(FILE *fp) {
    Rule* rules = malloc(sizeof(Rule));
    Action* action;
    Rule* curr=rules;
    int size=0;

    char* token;
    char temp_string[MAXLINE];
    if(fp == NULL) {
          perror("Unable to open file!");
          exit(1);
      }
      
    int index=0;
    while(fgets(temp_string, MAXLINE, fp) != NULL){
        index = 0;
        for(int i=0;i<=strlen(temp_string);i++){
            if(temp_string[i]!=' ' && temp_string[i]!='\t'){
                index=i;//index of first non-space,\t char
                break;
            }
        }
        if(temp_string[index]=='#'||temp_string[index]=='\n'||temp_string[index]=='\0'||temp_string[index]=='\r'){//temp is a comment line or empty line
            
            continue;
        }

        else if(index==0 && temp_string[0]!='\0'&& temp_string[0]!='\n' &&temp_string[0]!=' '){//target line
            
            token=strtok(temp_string," : ");
            char* target=malloc(strlen(token)+1);
            strcpy(target,token);
            Rule *rule=malloc(sizeof(Rule));
            rule->actions=NULL;
            rule->target=target;

            curr->next_rule=rule;
            curr=curr->next_rule;
 
            
            }
        else{
            action=malloc(sizeof(Action));
            action->next_act=NULL;
            char *words[MAXLINE];
            int i = 0;
        
            char *word = strtok(temp_string," ");
            while (word != NULL && i < MAXLINE) {
                while(*word==' '||*word=='\t'){
                    word+=1;
                }
                if (*word=='\n'||*word=='\r'||*word=='\0')
                {
                    word = strtok(NULL, " ");
                    continue;
                }
                while(word[strlen(word)-1]=='\n'||word[strlen(word)-1]=='\r'){
                    word[strlen(word)-1]='\0';
                }
                
                words[i] = malloc(strlen(word) + 1);
                strcpy(words[i], word);
                // printf("word %s\n",word);
                words[i][strlen(word)]='\0';
                word = strtok(NULL, " ");
                i++;
            }
            char **args=malloc((i+1)*sizeof(char*));
            for (int j = 0; j < i; j++) {
                args[j]=malloc(strlen(words[j])+1);
                strcpy(args[j],words[j]);
                // printf("args[j] %s\n",args[j]);
                free(words[j]);
            }
            args[i]=NULL;
            action->args=args;


            if(curr->actions==NULL){
                curr->actions=action;
            }
            else{
                Action* curr_action=curr->actions;
                while(curr_action->next_act!=NULL){curr_action=curr_action->next_act;}
                curr_action->next_act=action;

            }


        }
        

    }
    fseek(fp,0,SEEK_SET);
    Rule* last=rules;
    rules=rules->next_rule;
    free(last);
    last=curr;

    while(fgets(temp_string, MAXLINE, fp) != NULL){
        curr=rules;
        // printf("%s",temp_string);
        
        if (temp_string[0]!='\0'&&temp_string[0]!='\n'&&temp_string[0]!='#'&&temp_string[0]!='\t'&&temp_string[0]!='\r'){
            // printf("666\n");
            temp_string[strlen(temp_string)-1] = '\0';
            size=strlen(temp_string)-3;
            // printf("size%d",size);
            
            // printf("%s",temp_string);
            // printf("haha\n");


            token=strtok(temp_string," : ");
            // printf("%ld\n",strlen(token));
            // printf("%d\n",size);

            if(strlen(token)==size){

                    continue;
                }
            

            while(strcmp(curr->target,token)!=0){
                curr=curr->next_rule;
            }
            char* token2;
            token2=token;
            token2=strtok(token2+3+strlen(token2)," \r\n\t"); 

            while(token2!=NULL){
                if(strlen(token2)==1&&token2[0]=='\n'){break;}
                Rule* searcher = rules;
                while(searcher!=NULL && strcmp(searcher->target,token2)!=0){
                    // printf("seacher %s\n",searcher->target);
                    // printf("token%s\n",token2);

                    searcher=searcher->next_rule;
                    
                }
                // printf("\n done \n");

                
                
                if (searcher!=NULL)
                {
                Dependency *depend=malloc(sizeof(Dependency));
                depend->next_dep=NULL;
                depend->rule=searcher;
                if(curr->dependencies==NULL){
                    curr->dependencies=depend;
                }
                else{
                    Dependency* curr_depend =curr->dependencies;
                    while(curr_depend->next_dep!=NULL){
                        curr_depend=curr_depend->next_dep;
                        }
                        curr_depend->next_dep=depend;
                        


                    }
                    
                }
                else{
                    Rule * new_rule=malloc(sizeof(Rule));
                    char* new_target=malloc(strlen(token2)) ;
                    strcpy(new_target,token2);
                    new_rule->target=new_target;
                    new_rule->actions=NULL;
                    new_rule->dependencies=NULL;
                    new_rule->next_rule=NULL;

                    last->next_rule=new_rule;
                    last=last->next_rule;

                    Dependency *depend=malloc(sizeof(Dependency));
                    depend->next_dep=NULL;
                    depend->rule=new_rule;
                    if(curr->dependencies==NULL){
                    curr->dependencies=depend;
                        }
                    else{
                        Dependency* curr_depend =curr->dependencies;
                        while(curr_depend->next_dep!=NULL){
                        curr_depend=curr_depend->next_dep;
                        }
                        curr_depend->next_dep=depend;

                        }

                    


                }
                token2=strtok(NULL," \r\n\t");
                     
            }

        }
    }
    // printf("%s",(rules->actions->args[0]));
    // printf("end");

    
    return rules;
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
