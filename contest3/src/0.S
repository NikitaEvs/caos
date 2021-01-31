.data
	.global R
	R:
		.word
	
.text
	.global calculate

	calculate:
		PUSH 	{lr}
	
		LDR	r0, A_addr	/* r0 = &A */
		LDR	r0, [r0]	/* r0 = *r0 */

		LDR	r1, B_addr	/* r1 = &B */
		LDR	r1, [r1]	/* r1 = *r1 */
		
		LDR	r2, C_addr	/* r2 = &C */
		LDR	r2, [r2]	/* r2 = *r2 */

		LDR 	r3, D_addr	/* r3 = &D */
		LDR 	r3, [r3]	/* r3 = *r3 */ 

		MUL	r0, r1, r0	/* r0 = r1 * r0 or r0 = A * B */
		MUL	r2, r3, r2	/* r2 = r3 * r2 or r2 = C * D */

		ADD	r0, r2, r0	/* r0 = r2 + r0 or r0 = A * B + C * D */

		LDR	r3, R_addr
		STR	r0, [r3]

		POP	{pc}

A_addr:	.word A
B_addr:	.word B
C_addr: .word C
D_addr: .word D
R_addr: .word R
