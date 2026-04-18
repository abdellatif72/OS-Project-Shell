#include "Command.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 1024

Command *pwd_command();

int main() {
  // disable buffering for standard output -> always print immediately
  setvbuf(stdout, NULL, _IONBF, 0);

  // REPL
  while (1) {
    printf("shell$ ");

    // read the whole input of the user
    char user_input[MAX_LINE];
    if(fgets(user_input, sizeof(user_input), stdin) == NULL){
        printf("\n");
        break;
    }

    // remove the trailing newline
    user_input[strcspn(user_input, "\n")] = '\0';

    // skip empty input
    if(user_input[0]=='\0'){
        continue;
    }

    // spit by pipe into command list
    char *commands[50];
    int argc = 0;

    // read input till you meet "|" for the first time, that is your first command
    char *cmd = strtok(user_input, "|");
    while (cmd != NULL && argc < 49) {
      commands[argc++] = cmd;
      cmd = strtok(NULL, "|"); // passing NULL tells the strtok(): "continue from where you left off and read the next command"
    }

    commands[argc]=NULL;

    for(int i = 0; i < argc && commands[i]!=NULL; i++){
        char* name = strtok(commands[i], " \t");
        if(name==NULL) continue;

        Command* cmd = pwd_command();
        if(strcmp(name, cmd->name)==0){
            cmd->run(cmd); // execute the pwd command
            cmd->destroy(cmd); // free the memory
        }
    }
  }

  return 0;
}
