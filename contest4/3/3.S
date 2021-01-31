.intel_syntax noprefix

.text
.global dot_product

/* Input variables:
*  ebp + 8  : int N
*  ebp + 12 : const float *A
*  ebp + 16 : const float *B
*/
dot_product:
  PUSH      ebp
  MOV       ebp, esp            // ebp = esp, ebp = ptr to the top of stack

/* Local variables:
*  ebp - 4 : word result 
*/
  SUB       esp, 4              // allocate memory on the stack

 // MOV       eax, 1              // eax = 1
 // SUB       [ebp + 8], eax      // --N

  PXOR      xmm3, xmm3          // xmm3 = 0
  MOVSS     [ebp - 4], xmm3     // result = 0

Loop:
  MOV       eax, 4              // eax = 4
  CMP       [ebp + 8], eax      // cmp N and 4 
  JL        CarryLoop

  MOV       eax, [ebp + 8]      // eax = N
  MOV       edx, [ebp + 12]     // edx = A
  MOV       ecx, [ebp + 16]     // ecx = B

  MOVUPS    xmm0, [edx + 4*eax - 16] // unaligned packed mov floats in xmm0 
  MOVUPS    xmm1, [ecx + 4*eax - 16] // unaligned packed mov floats in xmm1
  DPPS      xmm0, xmm1, 0xFF    // dot product of xmm0 and xmm1 with mask 0b1111111 
  MOVSS     xmm3, [ebp - 4]     // xmm3 = result
  ADDSS     xmm3, xmm0          // xmm3 += xmm0
  MOVSS     [ebp - 4], xmm3     // result = xmm3 

  MOV       eax, 4              // eax = 1
  SUB       [ebp + 8], eax      // N -= 4 

  JMP       Loop

CarryLoop:
  MOV       eax, 0              // eax = 0
  CMP       [ebp + 8], eax      // cmp N and 0
  JE        EndLoop
  
  MOV       eax, [ebp + 8]      // eax = N
  MOV       edx, [ebp + 12]     // edx = A
  MOV       ecx, [ebp + 16]     // ecx = B

  MOVSS     xmm0, [edx + 4*eax - 4] // xmm0 = A[N]
  MOVSS     xmm1, [ecx + 4*eax - 4] // xmm1 = B[N]
  MULSS     xmm0, xmm1          // xmm0 *= xmm1
  MOVSS     xmm3, [ebp - 4]     // xmm3 = result
  ADDSS     xmm3, xmm0          // xmm3 += xmm0
  MOVSS     [ebp - 4], xmm3     // result = xmm3

  MOV       eax, 1              // eax = 1
  SUB       [ebp + 8], eax      // --N 

  JMP       CarryLoop

EndLoop:
  FLD       dword ptr [ebp - 4]  // st(0) = result 

  MOV       esp, ebp            // restore ptr to the top of stack (clear local variables)
  POP       ebp
  RET

