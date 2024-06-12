section .text
global enable_pae
global enable_paging

enable_pae:
    mov rax, cr4
    bts rax, 5
    mov cr4, rax
    ret

enable_paging:
    mov rax, cr0
    or rax, 0x80000000
    mov cr0, rax
    ret