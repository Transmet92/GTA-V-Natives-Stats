
StatNative PROTO C
EXTERN callptr:qword

.code
	TrampolineFunc proc
		mov   qword ptr [rbp-59h], rcx
		lea   rcx, [rbp - 69h]

		push rsp
		push rcx
		push rax
		push rdx
		push r8
		push r9
		push r10
		push r11

		mov rcx, r8
		call StatNative

		pop r11
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rax
		pop rcx
		pop rsp

		jmp callptr

		ret
	TrampolineFunc endp
end