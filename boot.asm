; Multiboot constants
MBALIGN equ 1<<0
MEMINFO equ 1<<1
FLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

; Multiboot header
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section bootstrap_stack, nobits
align 4
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

section .text
global _start
_start:
    mov esp, stack_top ; Set the stack pointer
    extern init
    push eax
    push ebx
    call init
.hang:
    hlt
    jmp .hang

global setGdt

gdtr DW 0 ; For limit storage
     DD 0 ; For base storage
 
setGdt:
   MOV   EAX, [esp + 4]
   MOV   [gdtr + 2], EAX
   MOV   AX, [ESP + 8]
   MOV   [gdtr], AX
   LGDT  [gdtr]
   RET

global reloadSegments
global reload_CS
reloadSegments:
   ; Reload CS register containing code selector:
   JMP   0x08:reload_CS ; 0x08 points at the new code selector
reload_CS:
   ; Reload data segment registers:
   MOV   AX, 0x10 ; 0x10 points at the new data selector
   MOV   DS, AX
   MOV   ES, AX
   MOV   FS, AX
   MOV   GS, AX
   MOV   SS, AX
   RET
