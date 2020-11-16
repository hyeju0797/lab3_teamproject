// > 처리
void start_stdin_redirect(char *file_path);

// pipe stdin처리
void pipe_stdin(int fd);

// stdin은 닫히면 안 열리기 때문에 복구
void recover_stdin();

// < 처리
void start_stdout_redirect(char *file_path);

//pipe stdout처리
void pipe_stdout(int fd);

//stdout은 닫히면 안 열리기 때문에 복구
void recover_stdout();
