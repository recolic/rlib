	.file	"cclass.c"
	.text
	.section	.rodata
.LC0:
	.string	"constructor called"
	.text
	.globl	vector_constructor
	.type	vector_constructor, @function
vector_constructor:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	push_back_rcpp_impl@GOTPCREL(%rip), %rdx
	movq	%rdx, (%rax)
	movq	-8(%rbp), %rax
	movq	at_rcpp_impl@GOTPCREL(%rip), %rdx
	movq	%rdx, 8(%rax)
	leaq	.LC0(%rip), %rdi
	call	puts@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	vector_constructor, .-vector_constructor
	.section	.rodata
.LC1:
	.string	"destructor called"
	.text
	.globl	vector_destructor
	.type	vector_destructor, @function
vector_destructor:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	leaq	.LC1(%rip), %rdi
	call	puts@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	vector_destructor, .-vector_destructor
	.section	.rodata
.LC2:
	.string	"Element at index %d is %d.\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$40, %rsp
	.cfi_offset 3, -24
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	leaq	-48(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	vector_constructor
	movq	-48(%rbp), %rax
	leaq	-48(%rbp), %rdx
	movl	$333, %esi
	movq	%rdx, %rdi
	call	*%rax
	movq	-48(%rbp), %rax
	leaq	-48(%rbp), %rdx
	movl	$666, %esi
	movq	%rdx, %rdi
	call	*%rax
	movq	-40(%rbp), %rax
	leaq	-48(%rbp), %rdx
	movl	$5, %esi
	movq	%rdx, %rdi
	call	*%rax
	movl	%eax, %edx
	movl	$5, %esi
	leaq	.LC2(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$123, %ebx
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	vector_destructor
	movl	%ebx, %eax
	movq	-24(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L5
	call	__stack_chk_fail@PLT
.L5:
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (GNU) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
