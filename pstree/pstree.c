#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[]) {

  pid_t pid_c=getpid();
  pid_t pid_c_p=getppid();
  printf("The current pid is: %d\n",pid_c);
  printf("The parent pid is: %d\n",pid_c_p);
  char filename[80]="/proc/1/stat";
  FILE *fp = fopen(filename, "r");
  if (fp) {
    // 用fscanf, fgets等函数读取
    printf("fp name:%s\n",fp->_cur_column);
    fclose(fp);
  } else {
    // 错误处理
  }


  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  return 0;
}
