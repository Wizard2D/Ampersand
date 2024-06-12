#pragma once

#include <stdint.h>
#include <stddef.h>
#include <libc/stdio.h>
#include <pic/pic.h>
#include <sched/timer.h>

#define PIC_FREQ 18

typedef struct __attribute__((packed))
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, error_code, rip, cs, rflags, rsp, ss;
} registers;

typedef struct  __attribute__((packed)){
   uint16_t offset_1; 
   uint16_t selector; 
   uint8_t  ist;            
   uint8_t  type_attributes; 
   uint16_t offset_2;   
   uint32_t offset_3;  
   uint32_t zero;     
} idt_desc;



typedef struct __attribute__((packed))
{
    uint16_t size;
    uint64_t addr;
} idtr;

static idt_desc entries[256] = {0};
static idtr idtptr = {0};
extern void* isr_stub_table[];

void idt_setdesc(uint64_t vec, void* isr, uint8_t flgs);
void idt_init();
void inthandler(registers *regs);