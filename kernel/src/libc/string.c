#include <libc/string.h>

uint32_t strlen(const char* str)
{
    uint32_t len = 0;

    while(str[len] != '\0')
    {
        len++;
    }

    return len;
}

char* itoa(char* str, int m){
    int tmp = 0;
    int f = 0;
    while(m != 0)
    {
        tmp = /*tmp * 10*/ m % 10;
        str[f++] = tmp+'0';

        m /= 10;
    }
    return strrev(str);
}

char* strrev(char* str)
{
    size_t strl = strlen(str);

    for(int i = 0; i < strl / 2; i++)
    {
        char ch = str[i];
        str[i] = str[strl - i - 1];
        str[strl - i - 1] = ch;
    }

    return str;
}