#include <gdt/gdt.h>
#include <libc/stdio.h>

#define PACKED __attribute__((packed))

typedef struct gdt_entry
{
    uint16_t  limit_low;
	uint16_t   base_low;
	uint8_t    base_mid;
	uint8_t      access;
	uint8_t limit_upper : 4;
	uint8_t       flags : 4;
	uint8_t  base_upper;
} PACKED;

typedef struct GDTPointer
{
	uint16_t size;
	uint64_t addr;
} PACKED;


static uint64_t gentries[3];
static uint8_t ent_ptr = 0;
static struct GDTPointer gpointer;


static void gdt_install()
{
	__asm__ volatile("lgdt %0\n"
		
		:: "m"(gpointer));
	__asm__ volatile ("call reloadSegments\n");
}

static void create_gdt_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	struct gdt_entry *ent = &gentries[ent_ptr++];

	ent->base_low = base &    0x0000FFFF;
	ent->base_mid = base &   (0x00FF0000) >> 16;
	ent->base_upper = base & (0xFF000000) >> 24;

	ent->access = access;
	ent->flags  =  flags;

	ent->limit_low = limit &    0x0000FFFF;
	ent->limit_upper = limit & (0x000F0000) >> 16;
}

void load_gdt()
{
	
	gentries[0] = 0x0000000000000000; // NULL.

	gentries[1] = 0x00af9b000000ffff; // CODE.
    
	gentries[2] = 0x00af93000000ffff; // DATA.

	gpointer.addr = (uint64_t)(uintptr_t)gentries;
	gpointer.size = sizeof(gentries) - 1;
	gdt_install();
	printf("GDT Installation Complete\n");
}