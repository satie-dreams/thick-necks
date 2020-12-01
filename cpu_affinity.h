#include "types.h"

#if NCPU <= 8
  typedef uchar cpu_set_t;
#define ALL_CPUS  0xFF
//#elif NCPU <= 16
//  typedef ushort CPU_SET;
//#elif NCPU > 32
//  typedef uint CPU_SET;
#endif

void CPU_ZERO(cpu_set_t *set);
void CPU_ALL(cpu_set_t *set);
void CPU_SET(int cpu, cpu_set_t *set);
int  CPU_ISSET(int cpu, cpu_set_t *set);
