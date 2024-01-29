void
init(void)
{
  // enable I-cache/D-cache, prefetch buffer (enable caches)
  // these could be disabled from a bootloader ...?
  // bootloader once in RAM must disable caches?
  // https://stackoverflow.com/questions/29739897/whats-the-reason-why-we-must-flush-data-cache-after-copy-code-from-flash-to-ram
  // https://stackoverflow.com/questions/21262014/arm-bootloader-disable-mmu-and-caches

  // set nvic priority grouping

  // enable systick (part of cmsis; TODO: CMSIS -> cortexm4 specific)

  // power peripheral voltage regulator

  // set this up for predictable/stable timing
  // (HSI) oscillator for clock source. This feeds into CPU, AHB and APB clocks
  // setup clock source for buses, e.g. AHB-> HSI, PLL
}
