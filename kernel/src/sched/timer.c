#include <sched/timer.h>

volatile int timer_count;
volatile int timer_seconds;

void pic_timer_setup()
{
    timer_count = 0;
    timer_seconds = 0;
}

void timer_inc()
{
    timer_count++;
}

void timer_reset()
{
    timer_count = 0;
}

int timer_getct()
{
    return timer_count;
}

int timer_getsec()
{
    return timer_seconds;
}

void timer_secinc()
{
    timer_seconds++;
}

void sleep(int seconds)
{
    uint64_t seconds_end = timer_getsec() + seconds;
    while(timer_getsec() < seconds_end)
    {
        __asm__ ("nop");
    }
}