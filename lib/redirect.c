#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "redirect.h"

int original_stdin = -1;
int original_stdout = -1;


//redirect,pipe 함수: dup2를 이용해서 구현
//recover 함수: dup2 이용해서 원래 stdin,stdout을 다시 연결

void start_stdin_redirect(char *file_path) {
  if (original_stdin == -1) original_stdin = dup(STDIN_FILENO);
  int fd = open(file_path, O_RDONLY|O_CREAT, 0644);
  dup2(fd, STDIN_FILENO);
  close(fd);
}

void pipe_stdin(int fd) {
  if (original_stdin == -1) original_stdin = dup(STDIN_FILENO);
  dup2(fd, STDIN_FILENO);
}

void recover_stdin() {
  if (original_stdin == -1) return;
  dup2(original_stdin, STDIN_FILENO);
  original_stdin = -1;
}

void start_stdout_redirect(char *file_path) {
  if (original_stdout == -1) original_stdout = dup(STDOUT_FILENO);
  int fd = open(file_path, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  dup2(fd, STDOUT_FILENO);
  close(fd);
}

void pipe_stdout(int fd) {
  if (original_stdout == -1) original_stdout = dup(STDOUT_FILENO);
  dup2(fd, STDOUT_FILENO);
}

void recover_stdout() {
  if (original_stdout == -1) return;
  dup2(original_stdout, STDOUT_FILENO);
  original_stdout = -1;
}
