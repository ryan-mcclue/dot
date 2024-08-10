global mov_all_bytes_asm
global nop_all_bytes_asm

section .text

; EXPORT void mov_all_bytes_asm(u8 *data, u32 count)
; sysV ABI: rax = f(rdi, rsi)
; volatile registers, so no need for callee to preserve
mov_all_bytes_asm:
  xor rax, rax
.loop:
  mov [rdi + rax], al
  inc rax
  cmp rax, rsi
  jb .loop
  ret

nop_all_bytes_asm:
  xor rax, rax
.loop:
  ; 3 byte NOP
  db 0x0f, 0x1f, 0x00
  ; will stall front-end
  nop
  nop
  nop
  nop
  nop
  nop
  inc rax
  cmp rax, rsi
  jb .loop
  ret

