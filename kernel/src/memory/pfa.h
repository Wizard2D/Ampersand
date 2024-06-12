#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGE_FRAME_SIZE 4096
typedef void* pageframe_t;

#define PAGE_FRAME_NUM 0x100000

#define PF_USED 0x1
#define PF_FREE 0x0

void pfbmp_init();
pageframe_t kalloc_frame();
void kfree_frame(pageframe_t frame);