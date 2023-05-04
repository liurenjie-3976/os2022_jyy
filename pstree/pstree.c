#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[]) {

  pid_t pid_c=getpid();
  printf("The current pid is: %d",pid_c);


  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  return 0;
}
