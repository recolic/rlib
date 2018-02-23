	.file	"cclass.c"
	.text
	.section	.rodata
.LC0:
	.string	"Element at index %d is %d."
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
	call	vector_constructor@PLT
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
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$123, %ebx
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	vector_destructor@PLT
	movl	%ebx, %eax
	movq	-24(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L3
	call	__stack_chk_fail@PLT
.L3:
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
