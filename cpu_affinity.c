#include "cpu_affinity.h"

void CPU_ALL(cpu_set_t *set) {
  *set = ALL_CPUS;
}

void CPU_ZERO(cpu_set_t *set) {
  *set = 0;
}

void CPU_SET(int cpu, cpu_set_t *set) {
  *set |= 1 << cpu;
}

int CPU_ISSET(int cpu, cpu_set_t *set) {
  return (*set & (1 << cpu)) >> cpu;
}
