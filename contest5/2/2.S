#include <asm/unistd_32.h>
.intel_syntax noprefix

.text
.global _start

_start:
/* First allocation */
  MOV ebp, esp

/* Local variables:
* buf     : [ebp - 4]     // buffer
* ptr     : [ebp - 8]     // pointer to the first free position                                      
* size    : [ebp - 12]    // size of the allocated space
* new_ptr : [ebp - 16]    // ptr for the reverse writing
*/                                                        
  SUB esp, 16       


  MOV eax, 0            // eax = 0
  MOV [ebp - 8], eax    // ptr = 0
  MOV eax, 4096         // eax = 4096
  MOV [ebp - 12], eax   // size = 4096

  MOV eax, __NR_brk     // eax = __NR_brk
  MOV ebx, 0            // ebx = 0
  
  INT 0x80              // brk system call

  MOV [ebp - 4], eax    // buffer ptr = eax
  ADD eax, [ebp - 12]   // buf += size
  MOV ebx, eax          // ebx = eax
  MOV eax, __NR_brk     // eax = __NR_brk

  INT 0x80              // brk system call
  
/* Reading from stdin */
ReadLoop:
  MOV eax, __NR_read    // eax = __NR_read 
  MOV ebx, 0            // ebx = 0 or stdin file description
  MOV ecx, [ebp - 4]    // ecx = buf
  ADD ecx, [ebp - 8]    // ecx = buf + ptr
  MOV edx, 4096         // edx = 4096

  INT 0x80              // read system call

  CMP eax, 0            // cmp eax and 0
  JLE EndReadLoop       // jmp to the end read loop if eax <= 0 

  ADD [ebp - 8], eax    // ptr += eax

  MOV eax, [ebp - 12]   // eax = *size
  CMP [ebp - 8], eax    // cmp *ptr and *size

  JL ReadLoop           // jmp if *ptr != *size

  MOV eax, [ebp - 12]   // eax = size
  ADD [ebp - 12], eax   // size *= 2

  MOV eax, __NR_brk     // eax = __NR_brk
  ADD ebx, [ebp - 4]    // ebx = buf
  ADD ebx, [ebp - 12]   // ebx += size
  
  INT 0x80              // brk system call
 
  JMP ReadLoop
EndReadLoop:
  MOV eax, 0            // eax = 0
  CMP [ebp - 8], eax    // cmp ptr and 0
  
  JG  SkipNormalize
  MOV eax, 10
  MOV ecx, [ebp - 4]
  MOV [ecx], eax
  MOV eax, __NR_write
  MOV ebx, 1
  MOV edx, 1

  INT 0x80
  JMP Exit

SkipNormalize:

  MOV eax, __NR_brk       // eax = __NR_brk
  ADD ebx, [ebp - 4]      // ebx = buf
  ADD ebx, [ebp - 12]     // ebx += size
  ADD ebx, 1              // ebx += 1
  
  INT 0x80                // brk system call

  MOV eax, [ebp - 4]      // eax = buf
  MOV edx, [ebp - 8]      // edx = ptr
  MOV ecx, 10             // ecx = '\n'
  MOV [eax + edx], ecx    // *(buf + ptr) = '\n'

SkipEnding:
  MOV eax, [ebp - 8]      // eax = ptr
  SUB eax, 1
  MOV [ebp - 16], eax     // new_ptr = ptr - 4

WriteLoop:
  MOV eax, [ebp - 16]     // eax = new_ptr
  CMP eax, 0              // eax = 0
  JL  EndWriteLoop        // jmp tp EndWriteLoop if new_ptr < 0 
  
  MOV eax, [ebp - 16]     // eax = new_ptr
  MOV edx, [ebp - 4]      // edx = buf
  MOV al, [edx + eax]     // eax = *(buf + new_ptr)

  CMP al, 10

  JNE ContinueWriteLoop

  MOV eax, __NR_write     // eax = __NR_write
  MOV ebx, 1              // ebx = 1 (stdout)
  MOV ecx, [ebp - 4]      // ecx = buf 
  ADD ecx, [ebp - 16]     // buf += new_ptr
  ADD ecx, 1              // buf += 1
  MOV edx, [ebp - 8]      // edx = ptr
  SUB edx, [ebp - 16]     // edx = ptr - new_ptr
  
  INT 0x80                // write system call 

  MOV eax, [ebp - 16]     // eax = new_ptr
  MOV [ebp - 8], eax      // ptr = new_ptr


ContinueWriteLoop:
  MOV eax, 1              // eax = 4
  SUB [ebp - 16], eax     // new_ptr -= 4

  JMP WriteLoop

EndWriteLoop:

MOV eax, __NR_write       // eax = __NR_write
MOV ebx, 1                // ebx = 1 (stdout)
MOV ecx, [ebp - 4]        // ecx = buf
ADD ecx, [ebp - 16]       // ecx += new_ptr 
ADD ecx, 1                // ecx += 1
MOV edx, [ebp - 8]        // edx = ptr
SUB edx, [ebp - 16]       // edx = ptr - new_ptr

INT 0x80                  // wite system call

Exit:

MOV eax, __NR_exit        // eax = __NR_exit
MOV ebx, 1                // ebx = 1
INT 0x80                  // exit system call
