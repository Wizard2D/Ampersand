#include <memory/virtmem.h>
#include <memory/pfa.h>
#include <libc/stdio.h>
#include <kernel.h>

extern struct limine_kernel_address_request kernaddress_req;
extern struct limine_memmap_request memmap;

extern uint64_t text_start;
extern uint64_t data_start;
extern uint64_t rodata_start;
extern uint64_t text_end;
extern uint64_t data_end;
extern uint64_t rodata_end;
extern uint64_t sbss;
extern uint64_t ebss;
extern uint64_t endkernel;

static uint64_t *page_dir_ptr_tab;
static uint64_t hhdm_offset = 0;


bool hhdmm = false;

#define phys_to_virt(addr) ((void *)((addr) + hhdm_offset))

#define virt_to_phys(addr) ((uint64_t)(addr) - hhdm_offset)

#define pte_getaddr(pte) ((pte) & PAGE_FRAME)

static void map_kernel()
{
    uint64_t paddr = kernaddress_req.response->physical_base;
    uint64_t vaddr = kernaddress_req.response->virtual_base;

    uint64_t _text_start = ALIGN_DOWN((uint64_t)&text_start, PAGE_SIZE);
    uint64_t _text_end = ALIGN_UP((uint64_t)&text_end, PAGE_SIZE);
    uint64_t _rodata_start = ALIGN_DOWN((uint64_t)&rodata_start, PAGE_SIZE);
    uint64_t _rodata_end = ALIGN_UP((uint64_t)&rodata_end, PAGE_SIZE);
    uint64_t _data_start = ALIGN_DOWN((uint64_t)&data_start, PAGE_SIZE);
    uint64_t _data_end = ALIGN_UP((uint64_t)&data_end, PAGE_SIZE);


    printf("Kernel Section Mapping\n");
    printf("Start: Data %x, Rodata %x, Text %x\n", _data_start, _rodata_start, _text_start);
    printf("End: Data %x, Rodata %x, Text %x\n", _data_end, _rodata_end, _text_end);

    for(uint64_t text = _text_start; text < _text_end; text += PAGE_SIZE)
    {
        map(text, text - vaddr + paddr, PAGE_PRESENT | PAGE_USER);
        printf(" t mapped %x to %x\n", text, text - vaddr + paddr);
    }
    for(uint64_t rodata = _rodata_start; rodata < _rodata_end; rodata += PAGE_SIZE)
    {
        map(rodata, rodata - vaddr + paddr, PAGE_PRESENT | PAGE_NX | PAGE_USER);
        printf(" rd mapped %x to %x\n", rodata, rodata - vaddr + paddr);
    }
    for(uint64_t data = _data_start; data < _data_end; data += PAGE_SIZE)
    {
        map(data, data - vaddr + paddr, PAGE_PRESENT | PAGE_NX | PAGE_RW | PAGE_USER);
        printf(" d mapped %x to %x\n", data, data - vaddr + paddr);
    }

    printf("mapped kernel");
}

void map_btldr()
{
    printf("mapping the bootloader\n");

    for(uint8_t i = 0; i < memmap.response->entry_count; i++)
    {
        struct limine_memmap_entry *entry = memmap.response->entries[i];
        if (entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            uint64_t vaddr = (uint64_t)phys_to_virt(entry->base);
            uint64_t paddr = entry->base;
            uint64_t pages = entry->length / PAGE_SIZE;
            for (uint64_t page = 0; page < pages; page++) {
                map(vaddr + (page * PAGE_SIZE), paddr + (page * PAGE_SIZE), PAGE_PRESENT | PAGE_RW | PAGE_USER);
            }
        }
    }
}

uint64_t get_largest_base()
{
    uint64_t high = 0;
    for (uint8_t i=0; i<memmap.response->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap.response->entries[i];
        if (entry->type != LIMINE_MEMMAP_RESERVED) {
            uint64_t this_high = entry->base + entry->length;
            if(this_high > high) high = this_high;
        }
    }
    return high;
}

void map_hhdm()
{
    uint64_t base = hhdm_offset;
    uint64_t high = get_largest_base();

    printf("mapping hhdm memory\n");
    hhdmm = true;
    for (uint64_t i = 0; i < high; i += PAGE_SIZE) {
        map(base + i, i, PAGE_PRESENT | PAGE_RW );
    }
}

void map_fdmem()
{
    printf("mapping found mem\n");
    for(size_t i = 0; i < memmap.response->entry_count; i++)
    {
        struct limine_memmap_entry *entry = memmap.response->entries[i];

        uint64_t vaddr = (uint64_t)phys_to_virt(entry->base);
        uint64_t paddr = entry->base;
        uint64_t pages = entry->length / PAGE_SIZE;

        if (entry->type != LIMINE_MEMMAP_USABLE) {
            continue;
        }

        for (uint64_t page = 0; page < pages; page++) {
            map(vaddr + (page * PAGE_SIZE), paddr + (page * PAGE_SIZE), PAGE_PRESENT | PAGE_RW | PAGE_USER);
        }
    }
}

extern void *memset(void *s, int c, size_t n);

void pgsetup()
{
    hhdm_offset = hhdm->offset;
    page_dir_ptr_tab = (uint64_t *)kalloc_frame();
    memset(page_dir_ptr_tab, 0, 512);
    map_btldr();
    map_fdmem();
    map_hhdm();
    map_kernel();

    load_pdpt();
}

static void flush_tlb() {
    asm volatile("mov %cr3, %rax; mov %rax, %cr3");
}

void map(uint64_t vaddr, uint64_t paddr, uint64_t flags)
{
    uint16_t pml4_index = (vaddr >> 39) & 0x1FF;
    uint16_t pml3_index = (vaddr >> 30) & 0x1FF;
    uint16_t pml2_index = (vaddr >> 21) & 0x1FF;
    uint16_t pml1_index = (vaddr >> 12) & 0x1FF;

    uint64_t *pml3 = NULL;
    if (page_dir_ptr_tab[pml4_index] & PAGE_PRESENT) {
        pml3 = phys_to_virt(PTE_GET_ADDR(page_dir_ptr_tab[pml4_index]));
    } else {
        pml3 = (uint64_t *)kalloc_frame(); 
        page_dir_ptr_tab[pml4_index] = virt_to_phys(pml3) | PAGE_PRESENT | PAGE_RW;
    }

    uint64_t *pml2 = NULL;
    if (pml3[pml3_index] & PAGE_PRESENT) {
        pml2 = phys_to_virt(PTE_GET_ADDR(pml3[pml3_index]));
    } else {
        pml2 = (uint64_t *)kalloc_frame(); 
        pml3[pml3_index] = virt_to_phys(pml2) | PAGE_PRESENT | PAGE_RW;
    }

    uint64_t *pml1 = NULL;
    if (pml2[pml2_index] & PAGE_PRESENT) {
        pml1 = phys_to_virt(PTE_GET_ADDR(pml2[pml2_index]));
    } else {
        pml1 = (uint64_t *)kalloc_frame(); 
        pml2[pml2_index] = virt_to_phys(pml2) | PAGE_PRESENT | PAGE_RW;
    }

    uint64_t perm_mod = pml1[pml1_index] & PAGE_PRESENT;
    pml1[pml1_index] = paddr | flags;

    if (perm_mod) {
        flush_tlb();
    }
}

void load_pdpt()
{
    printf("\nabt to load pdpt\n");
    printf("PDPT: Virtual %x, Physical %x, HHDM: %x", (uint64_t)&page_dir_ptr_tab, virt_to_phys((uint64_t)&page_dir_ptr_tab), hhdm_offset);
    asm volatile ("mov %0, %%cr3" :: "r" virt_to_phys(((uint64_t)&page_dir_ptr_tab)));
    printf("loaded pdpt\n");
}