#include <pic/pic.h>

void pic_remap(int offset1, int offset2)
{
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait(); 
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait(); 
    outb(PIC1_DATA, offset1); 
    io_wait(); 
    outb(PIC2_DATA, offset2); 
    io_wait(); 
    outb(PIC1_DATA, 4);  
    io_wait(); 
    outb(PIC2_DATA, 2);   
    io_wait(); 
    outb(PIC1_DATA, ICW4_8086);
    io_wait(); 
    outb(PIC2_DATA, ICW4_8086);
    io_wait(); 

    outb(PIC1_DATA, a1);
	outb(PIC2_DATA, a2);
}

void irq_mask_all() {
  for (int i = 0; i < 16; i++) irq_set_mask(i);
}

void pic_init()
{
    pic_remap(0x20,0x28);

    irq_mask_all();
    irq_clear_mask(0);
    irq_clear_mask(1);
    irq_clear_mask(2);
    
}
//.//.
void irq_set_mask(uint8_t IRQline)
{
    //a
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);  
}

void irq_clear_mask(uint8_t IRQline)
{
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value); 
}

uint16_t pic_get_IrqReg(int ocw3)
{
    outb(PIC1_CMD, ocw3);
    outb(PIC2_CMD, ocw3);
    return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

uint16_t pic_getIRR(void)
{
    return pic_get_IrqReg(PIC_READ_IRR);
}

uint16_t pic_getISR(void)
{
    return pic_get_IrqReg(PIC_READ_ISR);
}

void pic_send_eoi(uint8_t irq)
{

    if(irq >= 8)
		outb(PIC2_COMMAND, 0x20);
 
	outb(PIC1_COMMAND, 0x20);
}