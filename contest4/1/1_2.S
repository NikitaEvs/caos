.intel_syntax noprefix 
.text
.global my_sin

/* Input variables:
* x : [ebp + 8] 
*/
my_sin:
  PUSH    ebp               // save stack base
  MOV     ebp, esp

/* Local variables:
* k :       [ebp - 4]
* addition: [ebp - 12]
* result:   [ebp - 20]
*/
  SUB     esp, 20           // allocate space for local variables 
  
  MOV     eax, 0            // eax = 0
  MOV     [ebp - 4], eax    // k = 0
  MOVSD   xmm0, [ebp + 8]   // xmm0 = x
  MOVSD   [ebp - 12], xmm0  // addition = x

  PXOR    xmm0, xmm0        // xmm0 = 0 
  MOVSD   [ebp - 20], xmm0  // result = 0

Loop:
  MOVSD   xmm0, [ebp - 12]  // xmm0 = addition
  PXOR    xmm1, xmm1        // xmm1 = 0
  COMISD  xmm0, xmm1        // cmp xmm0 and xmm1
  JBE     EndLoop 

  MOV     eax, [ebp - 4]    // eax = k
  AND     eax, 1            // eax &= 1
  TEST    eax, eax          // cmp eax and 0

  JNE     NotEven   

  MOVSD   xmm0, [ebp - 20]  // xmm0 = result
  ADDSD   xmm0, [ebp - 12]  // xmm0 = result + addition
  MOVSD   [ebp - 20], xmm0  // result = result + addition

  JMP    Continue

NotEven:
  MOVSD   xmm0, [ebp - 20]  // xmm0 = result
  SUBSD   xmm0, [ebp - 12]  // xmm0 = result - addition
  MOVSD   [ebp - 20], xmm0  // result = result - addition

Continue:
  MOV     eax, 1            // eax = 1
  ADD     [ebp - 4], eax    // ++k

  MOVSD   xmm0, [ebp + 8]   // xmm0 = x
  MULSD   xmm0, xmm0        // xmm0 = x * x
  MOVSD   xmm1, [ebp - 12]  // xmm1 = addition
  MULSD   xmm1, xmm0        // xmm1 = addition * x * x
  MOVSD   [ebp - 12], xmm1  // addition *= x * x
  
  MOV     eax, [ebp - 4]    // eax = k
  ADD     eax, eax          // eax = 2*k
  MOV     edx, eax          // edx = eax = 2*k
  ADD     edx, 1            // edx = 2*k + 1
  IMUL    eax, edx          // eax = (2*k + 1)*2*k
  PXOR    xmm0, xmm0        // xmm0 = 0
  CVTSI2SD  xmm0, eax       // xmm0 = (double) eax
  MOVSD   xmm1, [ebp - 12]  // xmm1 = addition
  DIVSD   xmm1, xmm0        // xmm1 = addition /((2*k + 1)*2*k)
  MOVSD   [ebp - 12], xmm1  // addition = xmm1

  JMP     Loop

EndLoop:
  FLD     qword ptr [ebp - 20] // st(0) = result  

  MOV     esp, ebp          // clear local variables
  POP     ebp               // load stack base 
  RET

