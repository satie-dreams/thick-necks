#ifndef THICK_NECKS_UTHREAD_H
#define THICK_NECKS_UTHREAD_H
#include "types.h"
#include "thread.h"
#include "user.h"

void setesp(void *new_esp);
void setctx(void *new_esp);
void getctx(void **old_esp);

enum task_state { RUNNING, RUNNABLE, FINISHED };

typedef struct {
  enum task_state state;
  void *stack;
  void *esp;
  int started;
} Task;


typedef struct {
  int n_workers;
  int n_tasks;
  int n_run_tasks;
//  struct spinlock lock;
  Task *tasks;
  void *thread_ctx;
  int n_run_threads;
  int joining;
  char *msg;
} UThreadManager;

typedef struct {
  Task *task;
  Thread *thread;
  UThreadManager *utm;
} UThreadContext;

void uthread_task_init(UThreadManager *utm, void *func, void *arg);
void uthread_task_exit(UThreadManager *utm, Task* t);
void uthread_task_wrapper(
    UThreadManager*, Task*,
    void (*func)(UThreadManager*, Task*, void*), void *arg
);

UThreadContext *uthread_thread_context_create(UThreadManager *utm, void *func);
UThreadManager *uthread_create(int n_workers, int n_tasks);

void uthread_add_task(UThreadManager* utm, void* func, void *arg);
void uthread_schedule(UThreadContext *utc);
void uthread_yield(UThreadManager* utm, Task* t);
UThreadContext *uthread_get_utc(UThreadManager* utm, Task* t);
void uthread_join(UThreadManager* utm);

#endif //THICK_NECKS_UTHREAD_H
