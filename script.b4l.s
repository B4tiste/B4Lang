section .data
    format db "%d", 10, 0
section .bss
    x resq 1
    y resq 1
section .text
global _start
extern printf

_start:
    ; Affectation: x
    mov rax, 100
    mov [x], rax
    ; Affectation: y
    mov rax, 10
    mov [y], rax
    ; Condition IF
    mov rax, [x]
    push rax
    mov rax, [y]
    pop rbx
    cmp rbx, rax
    setg al
    movzx rax, al
    cmp rax, 1
    jne else_0
if_0:
    ; Return
    mov rax, [x]
    mov rsi, rax
    mov rdi, format
    mov rax, 0
    call printf
    mov rax, 60
    xor rdi, rdi
    syscall
    jmp end_1
else_0:
    ; Return
    mov rax, [y]
    mov rsi, rax
    mov rdi, format
    mov rax, 0
    call printf
    mov rax, 60
    xor rdi, rdi
    syscall
end_1:
