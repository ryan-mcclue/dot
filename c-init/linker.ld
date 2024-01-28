OUTPUT_FORMAT("elf32-littlearm")

FLASH_SIZE = 0x0
SRAM_SIZE = 0x0
BOOTLOADER_SIZE = 0x0
NVM_SIZE = 0x0

/* define with -DSTACK_SIZE=, as will play around with stack size */
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x800;

MEMORY 
{
  bootloader (rx) : ORIGIN = 0x00000000, LENGTH = BOOTLOADER_SIZE
  rom (rx) : ORIGIN = BOOTLOADER_SIZE, LENGTH = FLASH_SIZE - BOOTLOADER_SIZE - NVM_SIZE
  nvm (r) : ORIGIN = FLASH_SIZE - NVM_SIZE, LENGTH = NVM_SIZE
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = SRAM_SIZE
}

SECTIONS
{
  .text :
  {
    /* ARM elf file format mandates 4-byte alignment */
    . = ALIGN(4);
    /* KEEP as compiled with --gc-sections, but don't want to lose this */
    KEEP(*(.vectors .vectors.*))

    /* gcc uses standard section names in ELF ABI spec
     * of these standard names, adds own variations
    */
    *(.text .text.* .gnu.linkonce.t.*)
    *(.rodata .rodata* .gnu.linkonce.r.*)

    /* C-runtime, e.g. .init, .fini, .crt */
  } >rom



}

/* reset has to be at 0x000, however say flash starts at 0x200, can change at runtime with SCB->VTOR 
