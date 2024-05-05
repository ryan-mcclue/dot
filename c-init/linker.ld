/* NOTE: Desktop
SECTIONS
{
  .gnu_build_id : 
  {
    PROVIDE(g_note_build_id = .);
    *(.note.gnu.build-id)
  }
}
INSERT AFTER .rodata;
*/

OUTPUT_FORMAT("elf32-littlearm")

FLASH_SIZE = 0x0
SRAM_SIZE = 0x0
BOOTLOADER_SIZE = 0x0
NVM_SIZE = 0x0
/* NO HEAP SIZE? */

/* define with -DSTACK_SIZE=, as will play around with stack size */
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x800;

/* FLASH-REGIONS:
 * Designated region of flash (global struct->serialize->store):
   - heartbeat metrics 
   - execution flow/state (crashes, etc.)
   Periodically send to server database
   - device serial number (if OTP not available)
 */

/* RAM-REGIONS:
.noinit section in RAM is useful to preserve values on software reset.
e.g. on startup, mcu will typically set areas to 0

Move critical functions to zero-wait state RAM (i.e. TCM tight coupled memory)
*/

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
    /* _sfixed = .; */
    KEEP(*(.vectors .vectors.*))

    /* gcc uses standard section names in ELF ABI spec
     * of these standard names, adds own variations
    */
    *(.text .text.* .gnu.linkonce.t.*)
    _etext = .;
    /* constants, strings */
    *(.rodata .rodata* .gnu.linkonce.r.*)

    /* arm .extab (exception information) for stack unwinding, 
     * i.e functions that can throw exceptions will have entries here (or during debugging) */

    /* C-runtime, e.g. .init, .fini, .crt */
  } >rom

  .gnu_build_id : 
  {
    PROVIDE(_g_note_build_id = .);
    *(.note.gnu.build-id)
  } >rom

  /* TODO: why _sidata and _sdata? */
  _sidata = LOADADDR(.data);
  /* will start at flash, but want to put in ram so allocate space for it */
  .data :
  {
    . = ALIGN(4);
    /* if want to persist, store value in .word _sdata */
    _sdata = .;      
    *(.data)         
    *(.data*)        
    /* #define RAMFUNC __attribute__((section(".ramfunc")))
       void fast_function(void) RAMFUNC;
    */
    *(.RamFunc)      
    *(.RamFunc*)     

    . = ALIGN(4);
    _edata = .;

  } >ram AT> rom

  /* NOLOAD means that won't be loaded with a specific value (might be zeroed by startup or program loader), so don't need to keep in binary */
  .bss (NOLOAD):
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* The location of stack and heap can be swapped around depending on firmware choices */
  .stack (NOLOAD):
  {
     . = ALIGN(8);
     _sstack = .;
     . = . + STACK_SIZE;
     . = ALIGN(8);
     _estack = .;
  } > ram

  . = ALIGN(4);
  PROVIDE(_heap_start = .);
  _end = .; 
}

/* reset has to be at 0x000, however say flash starts at 0x200, can change at runtime with SCB->VTOR 
