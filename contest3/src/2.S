.text

.global main

main:
	PUSH	{lr}

	loop:
		LDR	r0, =stdin	/* r0 = &stdin */
		LDR	r0, [r0]	/* r0 = stdin */

		BL	fgetc
		
		CMP	r0, #0
		BLT	endloop
		
		CMP	r0, #48		/* r0 >=< '0' */
		BLT	loop
		CMP	r0, #57		/* r0 >=< '9' */
		BGT	loop

		LDR	r1, =stdout	/* r1 = &stdout */
		LDR	r1, [r1]	/* r1 = stdout */
		
		BL	fputc

		B loop

	endloop:
		
	POP	{lr}
  BX  lr

