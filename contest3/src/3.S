.data
	size:
		.word 0	

	position:
		.word 0
	
	ptr:
		.word 0

.text

.global main
.global stdin
.global stdout

read_byte:
	PUSH	{r4, lr}

	LDR 	r0, =stdin	/* r0 = &stdin */
	LDR	r0, [r0]	/* r0 = stdin */
	BL	fgetc
	MOV	r4, r0		/* r4 = r0 */
	CMP	r0, #0		/* r0 >=< 0 */
	BLT	end	

	/* Add element in the array and increment position */
	LDR	r1, =position	/* r1 = &position */
	LDR	r2, [r1]	/* r2 = *r1 or r2 = *position */

	LDR	r3, =ptr	/* r3 = &ptr */
	LDR	r3, [r3]	/* r3 = ptr */
	STR	r0, [r3, r2]	/* ptr[position] = r0 */ 

	ADD	r2, #1 		/* r2 = r2 + 1 */
	STR	r2, [r1]	/* *r1 = r2 */

	/* Check size */
	LDR	r3, =size	/* r3 = &size */
	LDR	r3, [r3]	/* r3 = *r3 */
	CMP	r2, r3		/* r2 >=< r3 */
	
	BEQ	reallocate
	
	B	end
	/* Reallocate array */
	reallocate:
		MOV	r2, #2		/* r2 = 2 */
		MUL	r3, r2, r3	/* r3 = r2 * r3 or r3 = 2 * r3 */
		LDR	r2, =size	/* r2 = &size */
		STR	r3, [r2]	/* *r2 = r3 or size = 2 * size */

		LDR 	r0, =ptr	/* r0 = &ptr */
		LDR	r0, [r0]	/* r0 = ptr */
		MOV	r1, r3		/* r1 = r3 or r1 = 2 * size */

		BL	realloc 
		LDR	r1, =ptr	/* r1 = &ptr */
		STR	r0, [r1]	/* *r1 = r0 or ptr = r0 */

	end:

	MOV	r0, r4		/* r0 = r4 */
	POP	{r4, pc}


main:
	PUSH 	{r4-r5, lr}

	/* Init allocation */
	MOV	r0, #0		/* r0 = 0 or ptr = NULL */
	MOV	r1, #2		/* r1 = 2 or init_size = 2 */
	LDR	r2, =size	/* r2 = &size */
	STR	r1, [r2]	/* *r2 = r1 or size = init_size */

	LDR	r2, =position	/* r2 = position */
	MOV	r3, #0		/* r3 = 0 */
	STR	r3, [r2]	/* *r2 = r3 or position = 0 */

	LDR	r1, =size	/* r1 = &size */
	LDR	r1, [r1]	/* r1 = *r1 or r1 = size */

	BL	realloc

	LDR	r1, =ptr	/* r1 = &ptr */
	STR	r0, [r1]	/* ptr = r0 */

	read_loop:
		BL	read_byte

		CMP	r0, #0		/* r0 >=< 0 */
		BGE	read_loop
	
	LDR	r5, =position	/* r5 = &position */
	LDR	r5, [r5]	/* r5 = *r5 or r5 = position */
	SUB	r5, r5, #1 	/* r5 = r5 - 1 */

	LDR	r4, =ptr	/* r2 = &ptr */
	LDR 	r4, [r4]	/* r2 = ptr */

	write_loop:
		LDR	r0, [r4, r5]	/* r0 = *(r4 + r5) or r0 = *(ptr + position) */
		LDR	r1, =stdout	/* r1 = &stdout */ 		
		LDR 	r1, [r1]	/* r1 = stdout */

		BL	fputc

		SUB	r5, r5, #1	/* r5 = r5 -1 */
		CMP	r5, #0
		BGE	write_loop

	/* Free pointer */
	MOV 	r0, r4		/* r0 = r4 */
	
	BL	free

	POP	{r4-r5, pc}
