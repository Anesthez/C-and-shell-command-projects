#define MAXLINE 256

typedef struct action_node {
    char **args;  // An array of strings suitable to be passed to execvp
    struct action_node *next_act;
} Action;

typedef struct dep_node {
    struct rule_node *rule;
    struct dep_node *next_dep;
} Dependency;

typedef struct rule_node {
    char *target;
    Dependency *dependencies;
    Action *actions;
    struct rule_node *next_rule;
} Rule;

/* Read from the open file fp, and create the linked data structure
   that represents the Makefile contained in the file.
   See the assignment handout for the specification of Makefile contents.
 */
Rule *parse_file(FILE *fp);

/* Return 1 if the line is a comment line, as defined on the assignment handout.
   Return 0 otherwise.
 */
int is_comment_or_empty(const char *line);

/* Concatenates args into a single string and store it in buffer, up to size.
   Return a pointer to buffer.
 */
char *args_to_string(char **args, char *buffer, int size);

/* Print the list of rules to stdout in makefile format */
void print_rules(Rule *rules);

/* Evaluate the rule in rules corresponding to the given target.
   If target is NULL, evaluate the first rule instead.
   If pflag is 0, evaluate each dependency in sequence.
   If pflag is 1, then evaluate each dependency in parallel (by creating one 
   new process per dependency). In this case, the parent process will wait until
   all child processes have terminated before checking dependency modified times
   to decide whether to execute the actions.
 */
void run_make(char *target, Rule *rules, int pflag);
