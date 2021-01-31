.intel_syntax noprefix

.text
.global summ

/* rdi - N, rsi - A, rdx - B, rcx - R */

summ:
  PUSHQ  rbx
  MOVQ   rbx, rcx  // rbx = rcx or rbx = R
  DEC rdi

Loop:
  DEC   rdi 
  MOVQ  rax, [rsi + 4*rdi] // eax = *(esi + 4*ecx) or eax = *(A + 4*ecx)
  ADDQ  rax, [rdx + 4*rdi] // eax += *(edx + 4*ecx) or eax = *(B + 4*ecx)
  MOVQ  [rbx + 4*rdi], rax // *(R + 4*ecx) = eax 
  CMP   rdi, 0
  JZ    Loop
  
  POPQ   rbx
  RET

