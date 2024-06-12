%macro PUSHAQ 0

PUSH RAX
PUSH RBX
PUSH RCX
PUSH RDX
PUSH RSI
PUSH RDI
PUSH RBP
PUSH R8
PUSH R9
PUSH R10
PUSH R11
PUSH R12
PUSH R13
PUSH R14
PUSH R15

%endmacro


%macro POPAQ 0

POP R15
POP R14
POP R13
POP R12
POP R11
POP R10
POP R9
POP R8
POP RBP
POP RDI
POP RSI
POP RDX
POP RCX
POP RBX
POP RAX

%endmacro

%macro isr_err_stub 1
isr_stub_%+%1:
    push %1
    jmp wrapper
%endmacro
; if writing for 64-bit, use iretq instead
%macro isr_no_err_stub 1
isr_stub_%+%1:
    push 0
    push %1
    jmp wrapper
%endmacro

extern inthandler
wrapper:
  cld
  PUSHAQ
  mov rdi, rsp
  call inthandler
  POPAQ
  add rsp, 16
  iretq

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

%assign i 32
%rep 224
isr_no_err_stub i
%assign i i+1
%endrep

section .data
global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    255
    DQ isr_stub_%+i ; use DQ instead if targeting 64-bit
%assign i i+1 
%endrep