#include <asm/unistd_32.h>
.intel_syntax noprefix

.data
outStr:
  .string "Hello, World!\n"

.text
.global _start

_start:
  PUSH  ebp
  MOV   ebp, esp

  MOV   eax, __NR_write   // eax = __NR_write or eax = 4
  MOV   ebx, 1            // ebx = 1 (stdout descriptor)
  LEA   ecx, outStr       // ecx = outStr 
  MOV   edx, 14           // edx = 14

  LEA   esi, write_ret
  PUSH  esi
  PUSH  ecx
  PUSH  edx
  PUSH  ebp
  MOV   ebp, esp

  sysenter

write_ret:

  MOV   eax, __NR_exit    // eax = __NR_exit or eax = 1
  MOV   ebx, 1            // ebx = 1

  PUSH  exit              // we don't care about it
  PUSH  ecx
  PUSH  edx
  PUSH  ebp
  
  MOV   ebp, esp

  sysenter

exit:

