format ELF64

public serve

section '.text' executable

serve:
    mov rax, 1
    mov rdi, 1
    mov rsi, msg
    mov rdx, msg_len
    syscall
    ret

section '.data'
  msg db 'Hi, from assembly', 0xA
  msg_len = $ - msg
