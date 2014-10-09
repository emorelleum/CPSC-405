	.file	"handle.c"
	.section	.rodata
.LC0:
	.string	"Nice try.\n"
	.text
	.globl	sigint_handler
	.type	sigint_handler, @function
sigint_handler:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	$1, -16(%ebp)
	movl	$10, 8(%esp)
	movl	$.LC0, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	write
	movl	%eax, -12(%ebp)
	cmpl	$10, -12(%ebp)
	je	.L1
	movl	$-999, (%esp)
	call	exit
.L1:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	sigint_handler, .-sigint_handler
	.section	.rodata
.LC1:
	.string	"exiting"
	.text
	.globl	sigusr1_handler
	.type	sigusr1_handler, @function
sigusr1_handler:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	$1, -16(%ebp)
	movl	$7, 8(%esp)
	movl	$.LC1, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	write
	movl	%eax, -12(%ebp)
	movl	$1, (%esp)
	call	exit
	.cfi_endproc
.LFE1:
	.size	sigusr1_handler, .-sigusr1_handler
	.section	.rodata
.LC2:
	.string	"%d\n"
.LC3:
	.string	"signal error: "
.LC4:
	.string	"Still here"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$32, %esp
	call	getpid
	movl	$.LC2, %edx
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	printf
	movl	$sigint_handler, 4(%esp)
	movl	$2, (%esp)
	call	signal
	cmpl	$-1, %eax
	jne	.L5
	movl	$.LC3, (%esp)
	call	puts
.L5:
	movl	$sigusr1_handler, 4(%esp)
	movl	$10, (%esp)
	call	signal
	cmpl	$-1, %eax
	jne	.L6
	movl	$.LC3, (%esp)
	call	puts
.L6:
	movl	$1, 16(%esp)
	movl	$.LC4, (%esp)
	call	puts
	leal	24(%esp), %eax
	movl	%eax, 4(%esp)
	leal	16(%esp), %eax
	movl	%eax, (%esp)
	call	nanosleep
	jmp	.L6
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
