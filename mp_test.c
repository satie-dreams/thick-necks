#include "types.h"
#include "stat.h"
#include "user.h"

__attribute__ ((noinline)) int dribble(int ncount) {
  float neg, a1, a2, a3, a4;
  a1 = 0;
  a2 = 0;
  a3 = 0;
  a4 = 0;
  neg = -1;

  for (int i = 1; i < ncount; i++) {
    a1 += neg + (float)i * 1.0/((2.0*(float)i)-1) + a2;
    a2 += neg + (float)i * 1.0/((2.0*(float)i)-1) + a3;
    a3 += neg + (float)i * 1.0/((2.0*(float)i)-1) + a4;
    a4 += neg + (float)i * 1.0/((2.0*(float)i)-1) + a1;
    neg = -i % 100 - a1;
  }

  return (int) a1 + 1;
}

void task(int idx, int fdwrite) {
  printf(1, "writing from %d\r\n", idx);

  int out = dribble(1e7);

  write(fdwrite, &out, 1);
}

int sequential(int ntasks) {
  int fds[2];
  pipe(fds);

  int buf = 0;
  int out = 0;

  for (int idx = 0; idx < ntasks; idx++) {
    task(idx, fds[1]);
  }

  for (int idx = 0; idx < ntasks; idx++) {
    read(fds[0], &buf, 1);
    out += buf;
  }

  return out;
}

int withprocesses(int ntasks) {
  int fds[2];
  pipe(fds);

  int buf = 0;
  int out = 0;

  for (int idx = 0; idx < ntasks; idx++) {
    if (fork() == 0) {
      close(fds[0]);
      task(idx, fds[1]);
      exit();
    }
  }

  for (int idx = 0; idx < ntasks; idx++) {
    if (wait() == -1) {
      printf(1, "We have only %d/%d loyal children\n", idx + 1, ntasks);
      break;
    }

    read(fds[0], &buf, 1);
    out += buf;
  }

  return out;
}

int withthreads(int ntasks) {
  int out = 0;

  int pid = thinfork();
  if (pid == 0) {
    out = 1;
    exit();
  } else if (pid == -1) {
    printf(1, "unsurprising news\n");
    exit();
  } else {
    thinwait();
  }

  return out;
}


int main() {
  int starttime, duration;
  int ntasks = 2;

  {
    printf(1, "-- With threading :)\n");

    starttime = uptime();
    int threaded_out = withthreads(ntasks);
    duration = uptime() - starttime;

    printf(1, "result = %d in %d time units\r\n", threaded_out, duration);
  }

  {
    printf(1, "-- With multiprocessing :)\n");

    starttime = uptime();
    int multi_out = withprocesses(ntasks);
    duration = uptime() - starttime;

    printf(1, "result = %d in %d time units\r\n", multi_out, duration);
  }

  {
    printf(1, "-- Sequential :(\n");

    starttime = uptime();
    int seq_out = sequential(ntasks);
    duration = uptime() - starttime;

    printf(1, "result = %d in %d time units\r\n", seq_out, duration);
  }

  exit();
}
