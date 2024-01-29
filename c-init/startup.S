.syntax unified // allows for easier .thumb and .arm usage
.cpu cortex-m4  // indicates thumbv2, dsp instructions etc.
.fpu softvfp // emulate fpu 
.thumb 
// IMPORTANT: in modern mcus, thumb is used as defacto
// availability determined by armv7-m
// 2 or 4 bytes (last bit 0) (thumb2 just adds more instructions to bring it closer to parity with ARM)
// indicative bit set in CPSR (Current Program State Register)
// IT instruction, instead of conditional instructions, e.g. ADDNE R1, R1, #1

.global  g_pfnVectors
.global  Default_Handler

// rely on assembler defaulting uninitialised to .data
.word  _sidata
.word  _sdata
.word  _edata
.word  _sbss
.word  _ebss

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function // give linker information
Reset_Handler: 
  ldr   sp, =_estack

  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0 // The trailing 's' sets CPSR register (typically considered with Zero and Negative flags)
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit
  
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

  bl  SystemInit   

  bl __libc_init_array

  bl  main
  bx  lr    
.size  Reset_Handler, .-Reset_Handler // directive not really necessary
