#define MAX_TOKEN_LENGTH 20

//입력 받은 string(=cmdline)을 분리해서 담음
typedef struct strtokens {
  char *elements[MAX_TOKEN_LENGTH];
  int length;
} strtokens_t;

//분리한 strtoken에 redirection, pipe가 있는지 확인하고 처리하기 위한 정보 담음
typedef struct cmdgroup {
  strtokens_t *tokens;
  char *stdin_redirect_file;
  char *stdout_redirect_file;
  char delimeter;
  int pipe_fd[2]; // for pipelining
} cmdgroup_t;

//cmdgroup, strtokens 초기화 하는 함수들
cmdgroup_t *create_cmdgroup();
strtokens_t *create_strtokens();

//strtokens에 분리한 것들을 추가해주는것
void add_element(strtokens_t *tokens, char *new_element);

//strtokens를 분리
strtokens_t *slice(strtokens_t *tokens, int start_idx, int end_idx);

//원래 코드의 makelist라고 생각하시면 됩니다(cmdline 분리)
void split(strtokens_t *tokens, char *input_str, const char *delimeters);
