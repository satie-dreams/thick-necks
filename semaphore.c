#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#define ACTIVE 1


struct semaphore
{
    int val;
    int active;
    struct spinlock lock;
} sem[ NSEMS ];


void
seminit(void)
{   
    for (int i=0; i<NSEMS;++i) {
        initlock(&sem[i].lock, "semaphore");
        sem[i].active = 0;
        sem[i].val    = 0;
    }
}

int
sem_init(int num, int max)
{
    if (num < 0 || num > NSEMS) return -1;

    acquire(&sem[num].lock);

    if (sem[num].active != 0) return -1;

    sem[num].active = ACTIVE;
    sem[num].val    = max;
    release(&sem[num].lock);

    return 0;
}

int 
sem_destroy(int num)
{
    if (num < 0 || num > NSEMS) return -1;
    
    acquire(&sem[num].lock);
    if (sem[num].active != ACTIVE) return -1;

    sem[num].active = 0;
    release(&sem[num].lock);

    return 0;
}

int 
sem_wait(int num, int count) 
{
    if (num < 0 || num > NSEMS) return -1;
    
    acquire(&sem[num].lock);

    if (sem[num].active != ACTIVE) return -1;

    while (sem[num].val <=  count - 1)
    {
        sleep(&sem[num], &sem[num].lock);
    }

    sem[num].val -= count;
    release(&sem[num].lock);

    return 0;    
}

int 
sem_signal(int num, int count)
{
    if (num < 0 || num > NSEMS) return -1;

    acquire(&sem[num].lock);

    if (sem[num].active != ACTIVE) return -1;

    sem[num].val += count;

    if (sem[num].val > 0) wakeup(&sem[num].lock);

    release(&sem[num].lock);

    return 0;
}

