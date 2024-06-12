#include <memory/pfa.h>
#include <kernel.h>

extern uint32_t endkernel;
static uint64_t start_address = 0;

uint64_t pfbmp[PAGE_FRAME_NUM / (sizeof(uint64_t) * 8)]; 

extern uint64_t get_largest_base();

void pfbmp_init()
{
    start_address = get_largest_base() + hhdm->offset;

    for(uint64_t i = 0; i < PAGE_FRAME_NUM / (sizeof(uint64_t) * 8); i++)
    {
        pfbmp[i] = PF_FREE;
    }
}

extern void *memset(void *s, int c, size_t n);
static pageframe_t kalloc_frame_int()
{
    uint64_t i = 0;
    while(pfbmp[i] != PF_FREE)
    {
        i++;
        if(i == PAGE_FRAME_NUM)
        {
            printf("No available page frames.\n");
            return -1; // replace with a kernel panic ASAP.
        }
    }
    pfbmp[i] = PF_USED;
    pageframe_t frame = start_address+(i*PAGE_FRAME_SIZE);

    memset((void*)frame, 0, PAGE_FRAME_SIZE);

    return frame;
}

static pageframe_t preframes[20];
static uint8_t allocate = 1;
static uint8_t pframe = 0;

pageframe_t kalloc_frame()
{
    pageframe_t ret;

    if(pframe == 20)
    {
        allocate = 1;
    }

    if(allocate == 1)
    {
        for(int i = 0; i < 20; i++)
        {
            preframes[i] = kalloc_frame_int();
        }
        pframe = 0;
        allocate = 0;
    }

    ret = preframes[pframe];
    pframe++;
    return ret;
}

void kfree_frame(pageframe_t frame)
{
    frame = frame - start_address;
    if(frame == 0)
    {
        pfbmp[0] = PF_FREE;
    }
    else
    {
        uint64_t index = ((uint64_t)frame)/PAGE_FRAME_SIZE;
        pfbmp[index] = PF_FREE;
    }
}