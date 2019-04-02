	.h8300h
	.section .text

	.global	_intr01
	.type	_intr01,@function
_intr01:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#1,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr02
	.type	_intr02,@function
_intr02:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#2,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr03
	.type	_intr03,@function
_intr03:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#3,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr04
	.type	_intr04,@function
_intr04:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#4,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr05
	.type	_intr05,@function
_intr05:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#5,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr06
	.type	_intr06,@function
_intr06:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#6,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr07
	.type	_intr07,@function
_intr07:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#7,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr08
	.type	_intr08,@function
_intr08:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#8,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr09
	.type	_intr09,@function
_intr09:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#9,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr10
	.type	_intr10,@function
_intr10:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#10,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr11
	.type	_intr11,@function
_intr11:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#11,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr12
	.type	_intr12,@function
_intr12:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#12,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr13
	.type	_intr13,@function
_intr13:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#13,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr14
	.type	_intr14,@function
_intr14:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#14,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr15
	.type	_intr15,@function
_intr15:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#15,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr16
	.type	_intr16,@function
_intr16:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#16,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr17
	.type	_intr17,@function
_intr17:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#17,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr18
	.type	_intr18,@function
_intr18:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#18,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr19
	.type	_intr19,@function
_intr19:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#19,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr20
	.type	_intr20,@function
_intr20:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#20,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr21
	.type	_intr21,@function
_intr21:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#21,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr22
	.type	_intr22,@function
_intr22:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#22,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr23
	.type	_intr23,@function
_intr23:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#23,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr24
	.type	_intr24,@function
_intr24:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#24,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr25
	.type	_intr25,@function
_intr25:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#25,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr26
	.type	_intr26,@function
_intr26:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#26,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr27
	.type	_intr27,@function
_intr27:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#27,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr28
	.type	_intr28,@function
_intr28:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#28,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr29
	.type	_intr29,@function
_intr29:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#29,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr30
	.type	_intr30,@function
_intr30:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#30,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr31
	.type	_intr31,@function
_intr31:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#31,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr32
	.type	_intr32,@function
_intr32:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#32,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr33
	.type	_intr33,@function
_intr33:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#33,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr34
	.type	_intr34,@function
_intr34:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#34,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr35
	.type	_intr35,@function
_intr35:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#35,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr36
	.type	_intr36,@function
_intr36:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#36,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr37
	.type	_intr37,@function
_intr37:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#37,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr38
	.type	_intr38,@function
_intr38:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#38,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr39
	.type	_intr39,@function
_intr39:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#39,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr40
	.type	_intr40,@function
_intr40:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#40,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr41
	.type	_intr41,@function
_intr41:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#41,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr42
	.type	_intr42,@function
_intr42:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#42,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr43
	.type	_intr43,@function
_intr43:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#43,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr44
	.type	_intr44,@function
_intr44:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#44,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr45
	.type	_intr45,@function
_intr45:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#45,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr46
	.type	_intr46,@function
_intr46:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#46,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr47
	.type	_intr47,@function
_intr47:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#47,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr48
	.type	_intr48,@function
_intr48:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#48,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr49
	.type	_intr49,@function
_intr49:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#49,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr50
	.type	_intr50,@function
_intr50:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#50,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr51
	.type	_intr51,@function
_intr51:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#51,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr52
	.type	_intr52,@function
_intr52:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#52,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr53
	.type	_intr53,@function
_intr53:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#53,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr54
	.type	_intr54,@function
_intr54:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#54,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr55
	.type	_intr55,@function
_intr55:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#55,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr56
	.type	_intr56,@function
_intr56:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#56,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr57
	.type	_intr57,@function
_intr57:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#57,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr58
	.type	_intr58,@function
_intr58:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#58,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr59
	.type	_intr59,@function
_intr59:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#59,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr60
	.type	_intr60,@function
_intr60:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#60,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr61
	.type	_intr61,@function
_intr61:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#61,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr62
	.type	_intr62,@function
_intr62:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#62,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

	.global	_intr63
	.type	_intr63,@function
_intr63:
	mov.l	er0,@-er7
	mov.l	er7,er0
	mov.l	#_stack,sp
	mov.l	er0,@-er7
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	mov.w	#63,r0
	jsr	@_interrupt
	mov.l	@er7+,er6
	mov.l	@er7+,er5
	mov.l	@er7+,er4
	mov.l	@er7+,er3
	mov.l	@er7+,er2
	mov.l	@er7+,er1
	mov.l	@er7+,er0
	mov.l	er0,er7
	mov.l	@er7+,er0
	rte

