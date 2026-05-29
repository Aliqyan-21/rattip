format ELF64

public serve

section '.text' executable

serve:
  push rbp
  mov rbp, rsp

  mov [public_dir], rdi ; rdi : public_dir
  mov [port], esi       ; esi : port
  mov ax, word [port]
  xchg al, ah
  mov [sin_port], ax
  mov [reload_gen], rdx ; rdx : reload_gen

  mov rax, 41 ; socket()
  mov rdi, 2
  mov rsi, 1
  mov rdx, 0
  syscall

  mov [sockfd], rax

  mov rax, 54 ; setsockopt() -- ignoring the return for now, we will use it when we implement error handling later
  mov rdi, [sockfd]
  mov rsi, 1
  mov rdx, 2
  mov r10, opt
  mov r8, 4
  syscall

  mov rax, 49 ; bind()
  mov rdi, [sockfd]
  mov rsi, sockaddr_in
  mov rdx, 16
  syscall

  mov rax, 50 ; listen()
  mov rdi, [sockfd]
  mov rsi, 10
  syscall

  mov rax, 34
  syscall

  pop rbp
  ret

section '.data' writeable
public_dir dq 0         ; char* (8 bytes)
port       dd 0         ; int (4 bytes)
reload_gen dq 0         ; int* (8 bytes)
sockfd     dq 0         ; int*
opt        dd 1         ; value is 1 (enable)
sockaddr_in:
sin_family dw 2         ; AF_INET 2 bytesr
sin_port   dw 0         ; filled in runtime, after port arg
sin_addr   dd 0         ; INADDR_ANY
sin_zero   dq 0         ; padding
