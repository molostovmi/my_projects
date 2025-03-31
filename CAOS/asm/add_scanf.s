.intel_syntax noprefix

.text
.global add_scanf

add_scanf:
  // prologue
  push rbp
  mov rbp, rsp
  sub rsp, 0x10

  lea rdi, [rip+scanf_format_string]
  lea rsi, [rsp+0x8]
  lea rdx, [rsp]

  call scanf

  xor rax, rax
  mov rax, [rsp+0x8]
  add rax, [rsp]

  // epilogue
  add rsp, 0x10
  pop rbp
  ret

.section .rodata

scanf_format_string:
  .string "%lld %lld"