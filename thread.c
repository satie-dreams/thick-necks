#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "thread.h"
#include "user.h"

Thread *thread_create(void* func, int* fd) {
  Thread *thread = malloc(sizeof(Thread));
  thread->stack = (void*) malloc(4096);
  thread->pid = clone(func, fd, thread->stack);
  thread->joinable = 1;
  return thread;
}

void thread_join(Thread *thread) {
  if (thread->joinable == 0) return;

  if (join(thread->pid) != thread->pid) {
    printf(2, "ERROR: problem while joining thread");
  }

//  printf(1, "Thread was joined\n");
  thread->joinable = 0;
}
