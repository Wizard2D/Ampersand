#include <idt/idt.h>

const char *exception_str[] = {
"Divide By Zero",
"Debug",
"Non-maskable Interrupt",
"Breakpoint",
"Overflow",
"Bound Range Exceeded",
"Invalid Opcode",
"Device Not Available",
"Double Fault",
"Coprocessor Segment Overrun",
"Invalid TSS",
"Segment Not Present",
"Stack-Segment Fault",
"General Protection Fault",
"Page Fault",
"RESERVED",
"x87 Floating-Point",
"Alignment Check",
"Machine Check",
"SIMD Floating-Point",
"Virtualization",
"Security Exception"
};

void idt_setdesc(uint64_t vec, void* isr, uint8_t flgs)
{
    idt_desc* desc = &entries[vec];

    desc->offset_1 = (uint64_t)isr & 0xFFFF;
    desc->selector = 0x8;
    desc->ist = 0;
    desc->type_attributes = flgs;
    desc->offset_2 = ((uint64_t)isr >> 16) & 0xFFFF;
    desc->offset_3 =  ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    desc->zero = 0;
}
//a//.

void idt_init()
{
    idtptr.size = (uint16_t)sizeof(entries)-1;
    idtptr.addr = (uintptr_t)&entries[0];

    for(uint64_t v = 0; v < 256; v++)
    {
        idt_setdesc(v, isr_stub_table[v], 0x8E);
    }
    
    __asm__ volatile ("lidt %0" :: "m"(idtptr));
    
    __asm__ volatile ("sti");

    pic_init();
    pic_timer_setup();
}




// EXCEPTION STUFF:
void inthandler(registers *regs)
{
    if(regs->int_no < 0x20){
        printf("Exception Caught: RIP -> %x || %s - %d : %x", regs->rip, exception_str[regs->int_no], regs->int_no, regs->error_code);
        __asm__ volatile("cli; hlt");
    }
    else
    {
        switch(regs->int_no)
        {
            case 0x20:
                timer_inc();
                if(timer_getct() == PIC_FREQ)
                {
                    timer_secinc();
                    timer_reset();
                }
                pic_send_eoi(0); 
                break;
            case 0x20+7:
                outb(0x20, 0x0B); 
                unsigned char irr = inb(0x20);
                pic_send_eoi(7);                
                break;
        }
    }
    
}