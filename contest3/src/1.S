.data

.balign 4
in_format_str:
	.string		"%d %d"
out_format_str:
	.string		"%d\n"
number1:
	.word 0
number2:
	.word 0

.text

.global main

main:
	PUSH	{lr}

	LDR	r0, =in_format_str
	LDR	r1, =number1 	/* r1 = &number1 */
	LDR	r2, =number2	/* r2 = &number2 */

	BL	scanf

	LDR	r1, =number1
	LDR	r1, [r1]
	LDR	r2, =number2
	LDR	r2, [r2]
	ADD	r1, r2, r1	/* r1 = r2 + r1 or r1 = number1 + number2 */	
	LDR	r0, =out_format_str

	BL	printf

	POP	{pc}	

