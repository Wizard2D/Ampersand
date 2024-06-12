#pragma once

#include <stdint.h>

void sleep(int seconds);
void pic_timer_setup();
void timer_inc();
void timer_reset();
int timer_getct();
int timer_getsec();
void timer_secinc();