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


void fork_cmd(char* argv[], int i, int fd[], int n) {
  pid_t pid;

  switch (pid = fork()) {
    case -1:
      fork_error();
    case 0:
      if (i == 0) {
          close(fd[READ]);   //read
          dup2(fd[WRITE], 1);
      } else if (i == n-1){
          close(fd[i*2+WRITE]);
          dup2(fd[(i-1)*2+READ], 0);
      } else {
          close(fd[(i-1)*2+WRITE]);   //write
          dup2(fd[(i-1)*2+READ], 0);
          dup2(fd[(i*2)+WRITE],1);
      }
      execvp(argv[0], argv);
      perror("execvp");
      exit(EXIT_FAILURE);
    default:
      close(fd[i*2+WRITE]);   //write
      //printf("I'm the parent and my PID is %ld \n", (long) getpid());
      break;
  }
}

void fork_cmds(char* argvs[MAX_COMMANDS][MAX_ARGV], int n) {
  int fd[2*n];
  for (int i = 0; i <n; i++){
   // int fd2[2];
   // fd[i] = fd2;
    pipe(&fd[2*i]);
  }

  for (int i = 0; i < n; i++){
    fork_cmd(argvs[i], i, fd, n);      //If we are at the last command!
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
