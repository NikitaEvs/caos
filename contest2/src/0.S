.text
.global f

f:
	MLA 	r1, r0, r3, r1	/* r1 = r0*r3 + r1 or r1 = A*x + B */
	MLA 	r0, r1, r3, r2	/* r0 = r1*r3 + r2 or r0 = (A*x + B)*x + C */
	bx 	lr
