#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "thread.h"
#include "user.h"

Thread *thread_create(void* func, void* arg) {
  Thread *thread = malloc(sizeof(Thread));
  thread->stack = (void*) malloc(USTACK_SIZE);
  thread->pid = clone(func, arg, thread->stack);
  thread->joinable = 1;
  return thread;
}

void thread_join(Thread *thread) {
  if (thread->joinable == 0) return;

  if (join(thread->pid) != thread->pid) {
    printf(2, "ERROR: problem while joining thread");
  }

  thread->joinable = 0;
}
