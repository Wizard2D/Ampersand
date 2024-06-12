#ifndef KERNEL_H
#define KERNEL_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

extern bool TerminalMode;

extern struct limine_framebuffer *framebuffer;
extern struct limine_hhdm_response *hhdm;

void kwrite(const char* str);

#endif