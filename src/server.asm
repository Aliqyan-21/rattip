format ELF64

public serve

section '.text' executable

serve:
  push rbp
  mov rbp, rsp

  mov [public_dir], rdi ; rdi : public_dir
  mov [port], esi       ; esi : port
  mov [reload_gen], rdx ; rdx : reload_gen

  mov rax, 41 ; socket()
  mov rdi, 2
  mov rsi, 1
  mov rdx, 0
  syscall

  mov [sockfd], rax

  pop rbp
  ret

section '.data' writeable
public_dir dq 0         ; char* (8 bytes)
port       dd 0         ; int (4 bytes)
reload_gen dq 0         ; int* (8 bytes)
sockfd     dq 0         ; int*
