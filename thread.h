#ifndef THICK_NECKS_THREAD_H
#define THICK_NECKS_THREAD_H

#define USTACK_SIZE   4096
typedef struct Thread {
  int pid;
  int joinable;
  void* stack;
} Thread;

Thread *thread_create(void* func, void* arg);
void thread_join(Thread *thread);
#endif //THICK_NECKS_THREAD_H
