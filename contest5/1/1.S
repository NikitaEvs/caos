#include <asm/unistd_32.h>
.intel_syntax noprefix

.data
buf: .word 0

.text
.global _start

_start:
Loop:
  MOV   eax, __NR_read    // eax = __NR_read
  MOV   ebx, 0            // ebx = 0 (stdin descriptor)
  LEA   ecx, buf          // ecx = buf
  MOV   edx, 1            // edx = 1

  INT   0x80              // syscall
  
  CMP   eax, 0            // cmp eax and 0
  JLE   EndLoop           // if eax <= 0 end loop

  MOV   eax, __NR_write   // eax = __NR_write 
  MOV   ebx, 1            // ebx = 1 (stdout descriptor
  LEA   ecx, buf          // ecx = buf
  MOV   edx, 1            // edx = 1

  INT   0x80

  JMP   Loop
                                             
                                              
EndLoop:                                      
  MOV   eax, __NR_exit    // eax = __NR_exit
  MOV   ebx, 1            // ebx = 1

  INT   0x80
                                              
