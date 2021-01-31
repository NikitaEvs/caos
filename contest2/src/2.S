.data

.balign 4
A_val: 		.word 0
B_val: 		.word 0
C_val: 		.word 0
D_val: 		.word 0

.text
/*
	Calculate Ax^3 + Bx^2 + Cx + D
*/
calculate:
	MOV	r1, r0 		/* r0 -> r1 or x -> r1 */
	
	LDR	r2, A_addr	/* r2 = &A_addr	*/
	LDR	r2, [r2]	/* r2 = *r2 */
	LDR 	r3, B_addr	/* r3 = &B_addr */
	LDR	r3, [r3] 	/* r3 = *r3 */
	
	MLA	r0, r2, r1, r3 	/* r0 = r2*r1 + r3 or r0 = A*x + B */
	
	LDR	r2, C_addr	/* r2 = &C_addr */
	LDR	r2, [r2]	/* r2 = *r2 */
	LDR	r3, D_addr	/* r3 = &D_addr */
	LDR 	r3, [r3]	/* r3 = *r3 */

	MLA	r0, r1, r0, r2 	/* r0 = r1*r0 + r2 or r0 = x*(A*x + B) + C */
	MLA	r0, r1, r0, r3 	/* r0 = r1*r0 + r3 or r0 = x(A*x^2 + Bx + C) + D */
	
	BX	lr	

.global solve
solve:
	PUSH	{r4, lr}

	LDR	r4, A_addr	/* r4 = &A_addr */
	STR	r0, [r4]	/* *r4 = r0 or *A_addr = A */
	LDR	r4, B_addr	/* r4 = &B_addr */
	STR	r1, [r4]	/* *r4 = r1 or *B_addr = B */
	LDR	r4, C_addr	/* r4 = &C_addr */
	STR	r2, [r4]	/* *r4 = r2 or *C_addr = C */
	LDR	r4, D_addr	/* r4 = &D_addr */
	STR	r3, [r4]	/* *r4 = r3 or *D_addr = D */

	MOV 	r4, #0		/* setup x */	

	loop:
		MOV	r0, r4			/* r4 -> r0 */
		BL	calculate
		
		CMP	r0, #0			/* if (r0 != 0) goto loop; */
		ADD	r4, r4, #1		/* ++r2 */
		BNE	loop	

	SUB	r4, r4, #1	/* r4 = r4 -1 */	
	MOV 	r0, r4		/* r2 -> r0 */	
	
	POP	{r4, pc}

A_addr: 	.word A_val
B_addr: 	.word B_val
C_addr: 	.word C_val
D_addr: 	.word D_val
