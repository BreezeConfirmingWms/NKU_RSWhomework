	.global _start
	
	.code16
	.text
DATA SEGMENT
	pkey DB 0DH 'HELLO OS SYSTEM'
DATA ENDS
_start:
	mov $0,%ax

	mov $0xb800, %ax
	mov %ax, %ds
	mov $160, %bx
	movb $'#', (%bx)
	movb $6, 1(%bx)

	jmp .
	
	.org 0x1fe
	.byte 0x55, 0xaa
