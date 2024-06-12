#include <kernel.h>
#include <libc/string.h>
#include <libc/stdio.h>
#include <font8.h>
#include <graphics/text_draw.h>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <memory/pfa.h>
#include <memory/virtmem.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

// Set the base revision to 2, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
struct limine_kernel_address_request kernaddress_req = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
struct limine_memmap_request memmap = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.

struct limine_framebuffer *framebuffer;
struct limine_hhdm_response *hhdm;
struct flanterm_context *ft_ctx;

void kwrite(const char* str)
{
    flanterm_write(ft_ctx, str, strlen(str));
}

bool TerminalMode = true;

void test_hhdm(void *hhdm) {
  return;
}

void _start(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    load_gdt();
 
    hhdm = hhdm_request.response;
    
    test_hhdm(hhdm->offset);

        // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
    || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    framebuffer = framebuffer_request.response->framebuffers[0];

    if(TerminalMode == false)
    {
        uint32_t twidth = 0, theight = 0;
        calc_text_size("WELCOME TO AMPERSAND\n", 3, &twidth, &theight);

        draw_str((volatile uint32_t*)framebuffer->address, framebuffer->pitch, font8, framebuffer->width/2-twidth/2, 25, "WELCOME TO AMPERSAND\n", create_color(255,0,0), 3);
        last_finish_x = 0;

        printf("This is a test of printf.");
        printf("This is written consecutively because I am very smart");
    }
    else
    {
        ft_ctx = flanterm_fb_init(
            NULL,
            NULL,
            (volatile uint32_t*)framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch,
            framebuffer->red_mask_size, framebuffer->red_mask_shift,
            framebuffer->green_mask_size, framebuffer->green_mask_shift,
            framebuffer->blue_mask_size, framebuffer->blue_mask_shift,
            NULL,
            NULL, NULL,
            NULL, NULL,
            NULL, NULL,
            NULL, 0, 0, 1,
            0, 0,
            0
        );

        printf("Welcome to Ampersand: Terminal Mode. Version 1.0-dev\n");
        printf("starting...\n");
        printf("intitializing paging...\n");
        printf("hhdm: %x", hhdm->offset);
        pfbmp_init();
        idt_init();
        printf("PFA done.\n");
        pgsetup();
        printf("paging enabled");
    }
    // We're done, just hang... 
    hcf();
}
