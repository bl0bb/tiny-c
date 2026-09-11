	.file	"main.c"
	.intel_syntax noprefix




	.text
	.globl	"rahh"
	.def	"rahh";	.scl	2;	.type	32;	.endef
	.seh_proc	"rahh"
"rahh":
	push	rbp
	.seh_pushreg	rbp
	mov	rbp, rsp
	.seh_setframe	rbp, 0
	sub	rsp, 16
	.seh_stackalloc	16
	.seh_endprologue
	mov	DWORD PTR -4[rbp], 5
	nop
	add	rsp, 16
	pop	rbp
	ret
	.seh_endproc




	.section .rdata,"dr"
.LC0:
	.ascii "Hola mi amigo\0"





	.text
	.globl	"tuffffff"
	.def	"tuffffff";	.scl	2;	.type	32;	.endef
	.seh_proc	"tuffffff"
"tuffffff":
	push	rbp
	.seh_pushreg	rbp
	mov	rbp, rsp
	.seh_setframe	rbp, 0
	sub	rsp, 32
	.seh_stackalloc	32
	.seh_endprologue
	mov	DWORD PTR 16[rbp], ecx
	lea	rax, .LC0[rip]
	mov	rcx, rax
	call	"puts"
	mov	eax, 0
	add	rsp, 32
	pop	rbp
	ret
	.seh_endproc





	.section .rdata,"dr"
.LC1:
	.ascii "Hello, World!\0"





	.text
	.globl	"main"
	.def	"main";	.scl	2;	.type	32;	.endef
	.seh_proc	"main"
"main":
	push	rbp
	.seh_pushreg	rbp
	mov	rbp, rsp
	.seh_setframe	rbp, 0
	sub	rsp, 96
	.seh_stackalloc	96
	.seh_endprologue
	mov	DWORD PTR 16[rbp], ecx
	mov	QWORD PTR 24[rbp], rdx
	call	"__main"
	lea	rax, .LC1[rip]
	mov	rcx, rax
	call	"puts"
	mov	BYTE PTR -1[rbp], 1
	mov	BYTE PTR -2[rbp], 2
	mov	BYTE PTR -3[rbp], 3
	mov	WORD PTR -6[rbp], 1
	mov	WORD PTR -8[rbp], 2
	mov	WORD PTR -10[rbp], 3
	mov	DWORD PTR -16[rbp], 1
	mov	DWORD PTR -20[rbp], 2
	mov	DWORD PTR -24[rbp], 3
	mov	DWORD PTR -28[rbp], 1
	mov	DWORD PTR -32[rbp], 2
	mov	DWORD PTR -36[rbp], 3
	mov	QWORD PTR -48[rbp], 1
	mov	QWORD PTR -56[rbp], 2
	mov	QWORD PTR -64[rbp], 3
	mov	eax, 0
	add	rsp, 96
	pop	rbp
	ret
	.seh_endproc




	
	.def	"__main";	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev5, Built by MSYS2 project) 16.1.0"
	.def	"puts";	.scl	2;	.type	32;	.endef
