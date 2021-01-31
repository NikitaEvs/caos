.intel_syntax noprefix

.text
.global mergesort


/* Input variables:
*  ebp + 8  : const int* in
*  ebp + 12 : int* out
*  ebp + 16 : int left 
*  ebp + 20 : int median
*  ebp + 24 : int right
*/
merge:
  PUSH  ebp
  MOV   ebp, esp          // ebp = esp, ebp = ptr to the top of stack

/* Local variables:
*  ebp - 4 : word iterLeft
*  ebp - 8 : word iterRight
*/
  SUB   esp, 8            // allocate memory on the stack

  MOV   eax, 0
  MOV   [ebp - 4], eax    // iterLeft = 0
  MOV   [ebp - 8], eax    // iterRight = 0

  /* Main loop */
Loop:
  MOV   eax, [ebp + 16]   // eax = left
  ADD   eax, [ebp - 4]    // eax = left + iterLeft
  CMP   eax, [ebp + 20]   // cmp left + iterLeft and median
  JGE   LeftPartLoop
  
  MOV   eax, [ebp + 20]   // eax = median
  ADD   eax, [ebp - 8]    // eax = median + iterRight
  CMP   eax, [ebp + 24]   // cmp median + iterRight and right
  JGE   LeftPartLoop
  
  MOV   eax, [ebp + 16]   // eax = left
  ADD   eax, [ebp - 4]    // eax = left + iterLeft
  MOV   edx, [ebp + 8]    // edx = in
  MOV   edx, [edx + 4*eax]// edx = in[left + iterLeft]
  MOV   eax, [ebp + 20]   // eax = median
  ADD   eax, [ebp - 8]    // eax = median + iterRight
  MOV   ecx, [ebp + 8]    // ecx = in
  MOV   eax, [ecx + 4*eax]// eax = in[median + iterRight]
  CMP   edx, eax          // cmp in[left + iterLeft] and in[median + iterRight]
  JGE   RightPart

  MOV   eax, [ebp - 4]    // eax = iterLeft
  ADD   eax, [ebp - 8]    // eax = iterLeft + iterRight
  MOV   ecx, [ebp + 12]   // ecx = out
  MOV   [ecx + 4*eax], edx// out[iterLeft + iterRight] = in[left + iterLeft]

  MOV   edx, 1
  ADD   [ebp - 4], edx    // ++iterLeft 

  JMP   Loop
  
RightPart:

  MOV   edx, [ebp - 4]    // edx = iterLeft
  ADD   edx, [ebp - 8]    // edx = iterLeft + iterRight
  MOV   ecx, [ebp + 12]   // ecx = out
  MOV   [ecx + 4*edx], eax// out[iterLeft + iterRight] = in[median + iterRight] 
  
  MOV   eax, 1
  ADD   [ebp - 8], eax    // ++iterRight

  JMP   Loop

LeftPartLoop:
  MOV   eax, [ebp + 16]   // eax = left
  ADD   eax, [ebp - 4]    // eax = left + iterLeft
  MOV   edx, [ebp + 20]   // edx = median
  CMP   eax, edx          // cmp left + iterLeft and median 
  JGE   RightPartLoop

  MOV   eax, [ebp + 16]   // eax = left
  ADD   eax, [ebp - 4]    // eax = left + iterLeft
  MOV   edx, [ebp + 8]    // edx = in
  MOV   ecx, [edx + 4*eax]// ecx = in[left + iterLeft]
  MOV   eax, [ebp - 4]    // eax = iterLeft
  ADD   eax, [ebp - 8]    // eax = iterLeft + iterRight
  MOV   edx, [ebp + 12]   // edx = out
  MOV   [edx + 4*eax], ecx// out[iterLeft + iterRight] = in[left + iterLeft]
 
  MOV   edx, 1
  ADD   [ebp - 4], edx    // ++iterLeft

  JMP   LeftPartLoop

RightPartLoop:
  MOV   eax, [ebp + 20]   // eax = median
  ADD   eax, [ebp - 8]    // eax = median + iterRight
  MOV   edx, [ebp + 24]   // edx = right
  CMP   eax, edx          // cmp median + iterRight and right
  JGE   EndLoop 
  
  MOV   eax, [ebp + 20]   // eax = median
  ADD   eax, [ebp - 8]    // eax = median + iterRight
  MOV   edx, [ebp + 8]    // edx = in
  MOV   ecx, [edx + 4*eax]// ecx = in[median + iterRight] 
  MOV   eax, [ebp - 4]    // eax = iterLeft
  ADD   eax, [ebp - 8]    // eax = iterLeft + iterRight
  MOV   edx, [ebp + 12]   // edx = out
  MOV   [edx + 4*eax], ecx// out[iterLeft + iterRight] = in[median + iterRight] 

  MOV   edx, 1
  ADD   [ebp - 8], edx    // ++iterRight

  JMP   RightPartLoop

EndLoop:
  MOV   ecx, 0

CopyLoop:
  MOV   eax, [ebp - 4]    // eax = iterLeft
  ADD   eax, [ebp - 8]    // eax = iterLeft + iterRight
  CMP   ecx, eax          // cmp ecx and iterLeft + iterRight
  JGE   Out

  MOV   eax, [ebp + 16]   // eax = left
  ADD   eax, ecx          // eax = left + ecx
  MOV   edx, [ebp + 12]   // edx = out 
  MOV   edx, [edx + 4*ecx]// edx = out[ecx]
  MOV   esi, [ebp + 8]    // esi = in
  MOV   [esi + 4*eax], edx// in[left + ecx] = out[ecx]

  INC   ecx

  JMP   CopyLoop

Out:
  MOV   esp, ebp          // restore ptr to the top of stack (clear local variables)
  POP   ebp
  RET

/* Input variables:
*  ebp + 8  : int from
*  ebp + 12 : int to
*  ebp + 16 : const int *in 
*  ebp + 20 : int *out
*/
mergesort:
  PUSH  ebp
  MOV   ebp, esp // ebp = esp, ebp = ptr to the top of stack

/* Local variables:
*  ebp - 4 : word i
*  ebp - 8 : word j
*/
  SUB   esp, 8   // allocate memory on the stack

  MOV   edx, 1
  MOV   [ebp - 4], edx // i = 1 

LengthLoop:
  MOV   edx, [ebp + 8]  // edx = from
  MOV   [ebp - 8], edx  // j = from
ShiftLoop:
  /* Call merge subroutine */
  MOV   edx, [ebp - 4]  // edx = i
  ADD   edx, edx        // edx = i + i
  ADD   edx, [ebp - 8]  // edx = 2*i + j
  CMP   edx, [ebp + 12] // cmp 2*i + j and to
  JL    CalculatedIndex

  MOV   edx, [ebp + 12] // if to < 2*i + j then edx = to

CalculatedIndex:
  MOV   eax, [ebp - 8]  // eax = j
  ADD   eax, [ebp - 4]  // eax = j + i

  PUSH  edx             // min(j + 2*i, to)
  PUSH  eax             // j + i
  PUSH  [ebp - 8]       // int j
  PUSH  [ebp + 20]      // int *out
  PUSH  [ebp + 16]      // const int *in

  CALL  merge

  ADD   esp, 20         // clear passing arguments from stack (5 * 4 bytes)

  /* ShiftLoop eniding */
  MOV   edx, [ebp - 4]  // edx = i
  ADD   edx, edx        // edx = i + i
  ADD   [ebp - 8], edx  // j += 2*i
  MOV   edx, [ebp + 12] // edx = to
  SUB   edx, [ebp - 4]  // edx = to - i 
  CMP   [ebp - 8], edx  // cmp j and to - i
  JL    ShiftLoop       // jump to ShiftLoop if j < to - i

  /* LengthLoop ending */
  MOV   edx, [ebp - 4]  // edx = i
  ADD   [ebp - 4], edx  // i = i + i
  MOV   edx, [ebp - 4]  // edx = i
  CMP   edx, [ebp + 12] // cmp i and to 
  JL    LengthLoop      // jump to LengthLoop if i < to

  /* Copy loop to answer */
  MOV   ecx, 0
CopyToAnswer:
  MOV   eax, [ebp + 12] // eax = to
  SUB   eax, [ebp + 8]  // eax = to - from
  CMP   ecx, eax
  JGE   Return  

  MOV   edx, [ebp + 8]  // edx = from
  ADD   edx, ecx        // edx = from + ecx
  MOV   eax, [ebp + 16] // eax = in
  MOV   eax, [eax + 4*edx] // eax = in[from + ecx]
  MOV   edx, [ebp + 20] // edx = out
  MOV   [edx + 4*ecx], eax // out[ecx] = in[from + ecx] 

  INC   ecx
  JMP   CopyToAnswer
  
Return:
  MOV   esp, ebp // restore ptr to the top of stack (clear local variables)
  POP   ebp
  RET

