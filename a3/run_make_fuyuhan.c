#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"
void run_action(char **args){
  int pid=fork();
  if(pid==-1){
        exit(1);
  }
  else if (pid == 0) {
    char command[MAXLINE];

    args_to_string(args, command,MAXLINE);

    printf("%s\n",command);
    int status_code = execvp(args[0],args);
    if (status_code == -1) {
      exit(1);
    }
    exit(0);
  }
  
  int status;

  if (wait(&status) != -1) {
    if (WIFEXITED(status)) {
      if(WEXITSTATUS(status)!=0){
        exit(WEXITSTATUS(status));
      }
    } 
  }
  
}

void run_actions(Rule* rule){

  Action* curr_action=rule->actions;
  while(curr_action!=NULL){
    run_action(curr_action->args);
    curr_action=curr_action->next_act;
  }
}

void run_actions1(Rule* rule){
  int action_counter=0;
  Action* curr_action=rule->actions;
  while(curr_action!=NULL){
    action_counter+=1;
    curr_action=curr_action->next_act;
  }
  char** args[action_counter];
  int i=0;
  curr_action=rule->actions;
  while(curr_action!=NULL){
    args[i]=curr_action->args;
    curr_action=curr_action->next_act;
    i+=1;
  }



  int exist_checker=0;
  for (int i = 0; i < action_counter; i++) {
    int result = fork();
    if (result < 0) {
            perror("fork");
            exit(1);
    } else if (result == 0) {
      run_action(args[i]);
      exit(0); 
      }
  }
  for(int i = 0; i < action_counter; i++) {
    int status;
    pid_t child_pid = wait(&status);
        if (child_pid == -1) {
            perror("wait");
            exit(1);
        }
        if (WIFEXITED(status)) {
          if(WEXITSTATUS(status)!=0){
            exist_checker+=1;
          }
        }
  }
  if (exist_checker>0)
  {
    exit(exist_checker);
  }
  
}

  
int update_check(Rule* curr,int num_depend){
  struct stat file_stat;
  int update_checker=0;
  struct stat depend_stat[num_depend];
  int i=0;
  stat(curr->target, &file_stat);

  Dependency *curr_dependency=curr->dependencies;
  while(curr_dependency!=NULL){
    stat(curr_dependency->rule->target, &(depend_stat[i]));
    curr_dependency=curr_dependency->next_dep;
    i+=1;
  }
  update_checker=0;
  time_t sectime = file_stat.st_mtim.tv_sec;
  time_t nanotime= file_stat.st_mtim.tv_nsec;
  for(int j=0; j<num_depend;j++){
    if (depend_stat[j].st_mtim.tv_sec>sectime ||(depend_stat[j].st_mtim.tv_sec==sectime&&file_stat.st_mtim.tv_nsec>nanotime)){
      update_checker=1;
      break;
    }
  }
  return update_checker;
}


void execute0(Rule *curr){
    int num_depend=0;
    struct stat file_stat;
    if (curr->dependencies==NULL){
      run_actions(curr);
    }
    else{
      Dependency * curr_dependency=curr->dependencies;
      while (curr_dependency!=NULL)
      {
        execute0(curr_dependency->rule);
        num_depend+=1;
        curr_dependency=curr_dependency->next_dep;
      }
      if (stat(curr->target, &file_stat) == -1) {
        run_actions(curr);
      }
      else if(update_check(curr,num_depend)!=0){
        run_actions(curr);
      }
      }
}

void execute1(Rule *curr){
  int num_depend=0;
  struct stat file_stat;
  if (curr->dependencies==NULL){
    run_actions1(curr);
    return;
  }
  else{
    Dependency * curr_dependency=curr->dependencies;
    while (curr_dependency!=NULL)
    {
      num_depend+=1;
      curr_dependency=curr_dependency->next_dep;
    }
    Rule* dependecies[num_depend];
    curr_dependency=curr->dependencies;
    for(int i =0; i<num_depend;i++)
    {
      dependecies[i]=curr_dependency->rule;
      curr_dependency=curr_dependency->next_dep;
    }
    for (int i = 0; i < num_depend; i++){
      int result = fork();
      if (result < 0) {
          perror("fork");
          exit(1);
      } else if (result == 0) {
        execute1(dependecies[i]);
        exit(0);
      }
  }
  int stat;
    for(int i =0; i<num_depend;i++){
      wait(&stat);
    }
  
  }
  if (stat(curr->target, &file_stat) == -1) {
    run_actions1(curr);
  }
  else if(update_check(curr,num_depend)!=0){
    run_actions1(curr);
  }



  

}
/* Evaluate the rule in rules corresponding to the given target.
   If target is NULL, evaluate the first rule instead.
   If pflag is 0, evaluate each dependency in sequence.
   If pflag is 1, then evaluate each dependency in parallel (by creating one 
   new process per dependency). In this case, the parent process will wait until
   all child processes have terminated before checking dependency modified times
   to decide whether to execute the actions.
 */

void run_make(char *target, Rule *rules, int pflag) {
    if(target==NULL){
        target=rules->target;
    }
    Rule* curr=rules;
    while(strcmp(curr->target,target)!=0){
        curr=curr->next_rule;
    }

    if (pflag==0){
        execute0(curr);
    }
    else{
        execute1(curr);
    }
}

