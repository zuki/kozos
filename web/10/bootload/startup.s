	.h8300h
	.section .text
	.global	_start
	.type	_start,@function
_start:
	mov.l	#_stack,sp
	jsr	@_main

_loop:
	bra	_loop
