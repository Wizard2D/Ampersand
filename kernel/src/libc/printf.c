#include <libc/string.h>
#include <libc/stdio.h>
#include <graphics/text_draw.h>
#include <font8.h>

char buffer[4096];
int bufUsed = 0;

void print_hex_impl(unsigned long long num, int nibbles) { for(int i = nibbles - 1; i >= 0; -- i) putch("0123456789ABCDEF"[(num >> (i * 4))&0xF]); }
#define print_hex(num) print_hex_impl((num), sizeof((num)) * 2)

void putch(char c)
{
    buffer[bufUsed++] = c;
}

void flush()
{
    buffer[0] = '\0';
    bufUsed = 0;
}

void putd(int x)
{
    char strdec[60];
    char* str = itoa(strdec, x);
    for(int i = 0; i < strlen(str); i++)
    {
        putch(str[i]);
    }
}

void puts(const char* str)
{
    if(TerminalMode == false)
        draw_str((volatile uint32_t *)framebuffer->address, framebuffer->pitch, font8, last_finish_x+10, 40, str, 0xFFFFFFFF, 2);
    else
        kwrite(str);
}

void pbuf()
{
    puts(buffer);
}


int printf(const char* fmt, ...)
{
    va_list list;
    va_start(list, fmt);

    int len = strlen(fmt);

    for(int i = 0; i < len; i++)
    {
        if(fmt[i] == '%')
        {
            switch(fmt[++i])
            {
                case 'c':
                {
                    int c = va_arg(list, int);
                    putch(c);
                    break;
                }
                case 's':
                {
                    char* str = va_arg(list, char*);
                    for(int i = 0; i < strlen(str); i++)
                    {
                        putch(str[i]);
                    }
                    break;
                }
                case 'd':
                {
                    int dcm = va_arg(list, int);
                    putd(dcm);
                    break;
                }
                case 'x':
                {
                    int hex = va_arg(list, int);
                    print_hex(hex);
                    break;   
                }
                case '%':
                {
                    putch('%');
                    break; 
                }       
            }
        }
        else
        {
            putch(fmt[i]);
        }
    }
    putch('\0');
    puts(buffer);
    flush();
    va_end(list);
    return 1;
}


