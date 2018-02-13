#include "parser.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#define READ  0
#define WRITE 1


void fork_error() {
  perror("fork() failed)");
  exit(EXIT_FAILURE);
}


void fork_cmd(char* argv[], int i, int fd[]) {
  pid_t pid;

  switch (pid = fork()) {
    case -1:

      fork_error();
    case 0:
      if (i == 1) {       //if it is the last command we let it read
          close(fd[WRITE]);   
          dup2(fd[READ], 0);
          //printf("The reader \n");
      } else {
          close(fd[READ]);   //All other commands will be allowed to write only
          dup2(fd[WRITE], 1);
      }
      execvp(argv[0], argv);
      perror("execvp");
      exit(EXIT_FAILURE);
    default:
      //printf("I'm the parent and my PID is %ld \n", (long) getpid());
      break;
  }
}

void fork_cmds(char* argvs[MAX_COMMANDS][MAX_ARGV], int n) {
  int fd[2];
  pipe(fd);
  for (int i = 0; i < n; i++) {
    if (i+1 == n) {
      fork_cmd(argvs[i], 1, fd);      //If we are at the last command!
    } else {
      fork_cmd(argvs[i], 0, fd);
    }
  }
  close(fd[WRITE]);
  for (int j = 0; j < n; j++) {
    //wait(NULL);
  }
}

void get_line(char* buffer, size_t size) {
  getline(&buffer, &size, stdin);
  buffer[strlen(buffer)-1] = '\0';
}

void wait_for_all_cmds(int n) {
  // Not implemented yet!

  for (int i = 0; i < n; i++) {
    wait(NULL);
  }
}

int main() {
  int n;
  char* argvs[MAX_COMMANDS][MAX_ARGV];
  size_t size = 128;
  char line[size];

  while(true) {
    printf(" >> ");
    get_line(line, size);

    n = parse(line, argvs);

    // Debug printouts.
    printf("%d commands parsed.\n", n);
    print_argvs(argvs);
    fork_cmds(argvs, n);
    wait_for_all_cmds(n);
  }

  exit(EXIT_SUCCESS);
}
