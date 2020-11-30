// thread.c
typedef struct Thread {
  int pid;
  int joinable;
  void* stack;
} Thread;

Thread *thread_create(void* func, int* fd);
void thread_join(Thread *thread);