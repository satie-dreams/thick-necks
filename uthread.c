#include "uthread.h"

UThreadManager *uthread_create(int n_workers, int n_tasks) {
  UThreadManager *utm = malloc(sizeof(UThreadManager));
  utm->n_workers = n_workers;
  utm->n_tasks = n_tasks;
  utm->n_run_tasks = 0;
  utm->n_run_threads = 0;
  utm->msg = "thread manager";
//  initlock(&utm->lock, "utm_lock");
  utm->tasks = (Task *) malloc(n_tasks * sizeof(Task));
  memset(utm->tasks, 0, sizeof(n_tasks * sizeof(Task)));
  utm->joining = 0;
  utm->thread_ctx = malloc(n_workers * sizeof(UThreadContext));
  for (int i = 0; i < n_workers; ++i) {
    uthread_thread_context_create(utm, uthread_schedule);
  }

  return utm;
}

void uthread_add_task(UThreadManager* utm, void* func, void *arg) {
  // TODO: mutex/spinlock
//  acquire(&utm->lock);
  uthread_task_init(utm, func, arg);
//  release(&utm->lock);
}

void uthread_task_wrapper(
    UThreadManager *utm, Task* t,
    void (*func)(UThreadManager *, Task*, void*), void *arg
) {
  // run user function
  func(utm, t, arg);

  // exit uthread
  uthread_task_exit(utm, t);
}


void uthread_schedule(UThreadContext *utc) {
  UThreadManager *utm = utc->utm;
  while (utm->n_run_tasks || !utm->joining) {
    for (int i = 0; i < utm->n_tasks; ++i) {
      Task* t = utm->tasks + i;

      if (t->state != RUNNABLE || utc->task == t) continue;

      printf(1, "found, state - %d\n", utc->task);

      t->state = RUNNING;

      // update thread context task
      Task *old_ctx_t = utc->task;
      utc->task = t;

      // if possible just set new context instead of switch
      if (!old_ctx_t || !t->started) {
        printf(1, "setting esp\n");
        t->started = 1;
        setesp(t->esp);
      } else {
        printf(1, "setting context\n");

        setctx(t->esp);
      }
    }

    // if this task was yielded by this thread (take it back)
    if (utc->task->state == RUNNABLE) {
      setctx(utc->task->esp);
    }
    sleep(1);
  }

  exit();
}

void uthread_join(UThreadManager* utm) {
  utm->joining = 1;

  for (int i = 0; i < utm->n_workers; ++i) {
    thread_join(((UThreadContext *) utm->thread_ctx + i)->thread);
  }

  free(utm->tasks);
  free(utm->thread_ctx);
}
void uthread_task_exit(UThreadManager *utm, Task *t) {
  printf(1, "thread exit...\n");
  // finish task
  utm->n_run_tasks--;
  t->state = FINISHED;

  // schedule this thread again
  UThreadContext *utc = uthread_get_utc(utm, t);
  uthread_schedule(utc);
}

void uthread_task_init(UThreadManager *utm, void *func, void *arg) {
  Task *t = utm->tasks + utm->n_run_tasks;

  printf(1, "init task i == %d, add - %d, size - %d\n", utm->n_run_tasks, t, sizeof(Task));
  t->stack = malloc(USTACK_SIZE);
  t->esp = t->stack + USTACK_SIZE;

  // arguments for uthread_task_wrapper
  *((int*) (t->esp -= 4)) = (int) arg;
  *((int*) (t->esp -= 4)) = (int) func;
  *((int*) (t->esp -= 4)) = (int) t;
  *((int*) (t->esp -= 4)) = (int) utm;
  // add return address (that should be never used)
  *((int*) (t->esp -= 4)) = (int) exit;
  // added wrapper for user task function
  *((int*) (t->esp -= 4)) = (int) uthread_task_wrapper;

  // mark this task active
  utm->n_run_tasks++;
  t->started = 0;
  t->state = RUNNABLE;
}

UThreadContext *uthread_thread_context_create(UThreadManager *utm, void *func) {
  UThreadContext *utc = utm->thread_ctx + utm->n_run_threads;
  printf(1, "init thread: %d\n", utc);
  utc->task = 0;
  utc->utm = utm;
  utc->thread = thread_create(func, utc);
  return utc;
}
UThreadContext *uthread_get_utc(UThreadManager* utm, Task* t) {
  for (int i = 0; i < utm->n_workers; ++i) {
    UThreadContext *utc = utm->thread_ctx + i;
    if (utc->task == t) {
      return utc;
    }
  }

  printf(2, "uthread_get_utc: ERROR: user thread context not found\n");
  exit();
}

void uthread_yield(UThreadManager* utm, Task* t) {
  // a bit dirty solution
  volatile int yielded = 0;

  printf(1, "thread yielded ...\n");
  printf(1, "t->esp = %d\n", t->esp);
  getctx(&t->esp);

  printf(1, "t->esp = %d, yielded = %d\n", t->esp, yielded);

  if (!yielded) {
    yielded = 1;
    UThreadContext *utc = uthread_get_utc(utm, t);
    t->state = RUNNABLE;
    uthread_schedule(utc);
  }
}