.text

.global summ

summ:
	loop:
		SUB	r1, r1, #1	/* r1 = r1 - 1 or N = N - 1 */
		LDR 	r3, [r2], #4	/* r3 = *r2; ++r2; or r3 = *(X++); offset = 4 bytes */
		ADD	r0, r0, r3	/* r0 = r0 + r3 */
		CMP	r1, #0		/* if (r1 != 0) goto loop; */
		BNE	loop
	BX	lr	
