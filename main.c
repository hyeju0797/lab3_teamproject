#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "lib/cmd.h"
#include "lib/redirect.h"

#define BUFSIZE 256
#define MAX_CMD_GROUP 10

char cmdline_str[BUFSIZE];

const char *prompt = "myshell> ";

//좀비방지
static void child_handler(int _){
  int status;
  pid_t killedpid;
  while((killedpid=(waitpid(-1,&status,WNOHANG)))>0);
  return;
}

//ctrl+C quit 처리
static void signal_handler(int _) {
}


void fatal(char *str){
   perror(str);
   exit(1);
}

void run_command(cmdgroup_t *group) {
  strtokens_t *tokens = group->tokens;
  char *first_command = group->tokens->elements[0];
  int length = tokens->length;

  if (length == 0) {
    return;
  }
  // exit command
  if(strcmp(first_command, "exit") == 0) {
    printf("exit shell!\n");
    exit(0);
  }
  // redirection 
  if (group->stdout_redirect_file != NULL) {
    start_stdout_redirect(group->stdout_redirect_file);
  }
  if (group->stdin_redirect_file != NULL) {
    start_stdin_redirect(group->stdin_redirect_file);
  }

  // cd
  if (strcmp(first_command, "cd") == 0) {
    if ((length < 2) || (strcmp(tokens->elements[1], "~") == 0)) {
      char* homedir = getenv("HOME");
      chdir(homedir);
    } else {
      chdir(tokens->elements[1]);
    }

    return;
  }

  // others
  int should_pipeline = group->delimeter == '|';
  int is_background = strcmp(tokens->elements[length-1], "&") == 0;
  int is_sleep_command = strcmp(tokens->elements[0], "sleep") == 0;
  int sleep_delay = 0;

  if (should_pipeline) pipe(group->pipe_fd);
  if (is_background) {
    tokens->elements[length - 1] = NULL;
    length--;
  }
  if (is_sleep_command) {
    sleep_delay = atoi(tokens->elements[1]);
  }

  signal(SIGCHLD,(void*) child_handler);
  pid_t pid = fork();

  if (pid == -1) {
    fatal("main()");

  } else if(pid == 0){
    // child 
    if (should_pipeline) {
      close(group->pipe_fd[0]);
      pipe_stdout(group->pipe_fd[1]);
    }
    if(is_sleep_command) {
      sleep(sleep_delay);
      exit(0);
    } 
    else{
      execvp(tokens->elements[0], tokens->elements);
      fatal("main()");
    }

  } else {
    // parent 
    if (should_pipeline) {
      close(group->pipe_fd[1]);
      pipe_stdin(group->pipe_fd[0]);
    }
    if (!is_background) {
      wait(NULL);
    }
  }
}

void handle_cmdline_tokens(strtokens_t *cmd_tokens) {
  cmdgroup_t *cmd_groups[MAX_CMD_GROUP];
  int num_groups = 0;

  int group_start_idx = 0;
  cmdgroup_t *group = NULL;

  //나눈 strtoken에 pipe,redirection 있으면 그걸 묶어줌
  for (int i = 0; i < cmd_tokens->length; i++) {
    // char로 strtoken을 확인
    char delimeter = cmd_tokens->elements[i][0];

    if ((delimeter != '|') && 
        (delimeter != '>') &&
        (delimeter != '<')) {
      continue;
    }

    // redirection, pipe 없을때 
    if (group == NULL) {
      group = create_cmdgroup();
      group->tokens = slice(cmd_tokens, group_start_idx, i);
      cmd_groups[num_groups++] = group;
    }

    //pipe 있을때
    if (delimeter == '|') {
      // | 다음에 올 인자로 새로운 그룹 만듬
      group->delimeter = delimeter;
      group = NULL;

    } 
    //redirection
    else if (delimeter == '>') {
      // output redirection
      char *next_token = cmd_tokens->elements[++i];
      group->stdout_redirect_file = next_token;

    } else if (delimeter == '<') {
      //input redirection
      char *next_token = cmd_tokens->elements[++i];
      group->stdin_redirect_file = next_token;
    }

    group_start_idx = i + 1;
  }

  // 마지막 그룹 추가
  if (group_start_idx < cmd_tokens->length) {
    group = create_cmdgroup();
    group->tokens = slice(cmd_tokens, group_start_idx, cmd_tokens->length);
    cmd_groups[num_groups++] = group;
  }

  //그룹 분리를 끝내고 실제 명령어 실행
  int i = 0;
  while (i < num_groups) {
    cmdgroup_t *group = cmd_groups[i++];
    run_command(group);
  }
}

int main() {
  signal(SIGINT,(void*)signal_handler);
  signal(SIGQUIT,(void*)signal_handler);

  while (1) {
    // recover stdin, stdout(stdin,stdout은 한 번 닫히면 안 열리기 떄문)
    recover_stdin();
    recover_stdout();

    //명령어 입력
    fputs(prompt, stdout);
    fgets(cmdline_str, BUFSIZE, stdin);
    cmdline_str[strlen(cmdline_str) - 1] = '\0';

    //받은 명령어(=cmdline)을 공백 기준으로 strtoken으로 나눈다
    strtokens_t *cmd_tokens = create_strtokens();
    split(cmd_tokens, cmdline_str, " ");

    // enter 입력시 처리
    if (cmd_tokens->length < 1) continue;

    //나눈 명령어 실행
    handle_cmdline_tokens(cmd_tokens);
  }
}
