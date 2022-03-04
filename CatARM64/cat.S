.section .data
.section .bss
	.comm buffer, 4096, 4 #buffer .comm = common symbol, $buffer for address not $buf
.section .text
	.globl _start

_start:

readwriteloop:

	mov	$0,%rax		#0-read syscall, load into rax
	mov	$buffer,%rsi	#address of temporary buffer
	mov	$4096,%rdx	#size of buffer
	mov	$0, %rdi	#fd-0 stdin, we read from stdin
	syscall		#send read syscall previously loaded in

	cmp	$0, %rax	#compare 0 to value in eax
	je	exit		#if equal, EOF, exit with exit code 0
	jl	exiterror	#if less than 0, read failed, exit with exit code -1

	mov	%rax,%rdx	#return val of read should be thirs argument of write
	mov	$1,%rax		#move syscall 1-write
	mov	$1,%rdi		#first argument, fd-stdout-1
	mov	$buffer,%rsi	#2nd argument, buffer
	syscall

	jmp	readwriteloop
exit:

	mov	$60,%rax	# Load syscall 60-exit 
	mov	$0,%rdi		#first argument: exit code we want
	syscall		#sends exit syscall

exiterror:
	mov	$60,%rax	#Load syscall 60-exit
	mov	$-1,%rdi	#exit code -1
	syscall		#send exit syscall
