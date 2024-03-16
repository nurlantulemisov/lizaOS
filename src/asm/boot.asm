; bootloader

[bits 32]

; constants for multiboot header
MBALIGN     equ  1<<0                   ; выровнить по границе страницы
MEMINFO     equ  1<<1                   ; установить memory map
; BOOTDEVICE  equ  1<<1
FLAGS       equ  MBALIGN | MEMINFO      ; multiboot flags
MAGIC       equ  0x1BADB002             ; magic for finding bootloader header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot


; kernel entry, main text section
section .text
align 4
; set multiboot section
section .multiboot
    ; Говорим загрузчику, что это есть kernel по MAGIC. 
    ; Bootloader будет искать эту запись в первых 8кб
    ; https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-sources
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

global _start

_start:
    mov esp, stack_top
    extern kmain
    mov eax, MAGIC
    push ebx
    push eax
    call kmain
loop:
    hlt
    jmp loop

section .bss
align 16                        ; The stack on x86 must be 16-byte aligned according to the
                                ; System V ABI standard and de-facto extensions. The compiler will assume the
                                ; stack is properly aligned and failure to align the stack will result in
                                ; undefined behavior.


stack_bottom:
    ; 16KiB of uninitialized data for stack
    resb 16384
stack_top:


