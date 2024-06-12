#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGE_PRESENT    0x1     // Present (P) flag
#define PAGE_RW         0x2     // Read/Write (R/W) flag
#define PAGE_USER       0x4     // User/Supervisor (U/S) flag
#define PAGE_ACCESSED   0x20    // Accessed (A) flag
#define PAGE_DIRTY      0x40    // Dirty (D) flag
#define PAGE_PS         0x80    // Page Size (PS) flag
#define PAGE_GLOBAL     0x100   // Global (G) flag
#define PAGE_NX         (1ULL << 63) // No-Execute (NX) flag for x86-64

#define PAGE_SIZE       4096
#define PAGE_FRAME      (~(PAGE_SIZE - 1))



#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)
#define PTE_GET_FLAGS(VALUE) ((VALUE) & ~PTE_ADDR_MASK)

#define DIV_ROUND_UP(x, y) (x + (y - 1)) / y
#define ALIGN_UP(x, y) DIV_ROUND_UP(x, y) * y
#define ALIGN_DOWN(x, y) (x / y) * y // thanks meow

#define aligned(x) __attribute__((aligned(x)))

void pgsetup();
void load_pdpt();
void map(uint64_t vaddr, uint64_t paddr, uint64_t flags);
extern void enable_pae();
extern void enable_paging();

