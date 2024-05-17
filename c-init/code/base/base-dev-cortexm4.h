// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_DEV_CORTEXM4_H)
#define BASE_DEV_CORTEXM4_H

#include "external/cmsis/core_cm4.h"

// TODO(Ryan): cmsis uses dsp for simd? (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
// uqadd8, so can do parallel 8-bit math?

// NOTE(Ryan): No nested interrupts at all
#define ATOMIC_BEGIN() __disable_irq()
#define ATOMIC_END() __enable_irq()

// NOTE(Ryan): Using an exception mask register, allow nested interrupts in the form of NMI and HardFault
GLOBAL volatile u32 global_primask_reg;
#define CRITICAL_BEGIN() \
    do { \
        global_primask_reg = __get_PRIMASK(); \
        __set_PRIMASK(1); \
    } while (0)
#define CRITICAL_END() \
    do { \
        __set_PRIMASK(global_primask_reg); \
    } while (0)

// more useful for dual core
#define ARE_IN_IRQ() \
  (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)

#define SYSCLK_MHZ() \
  ((u16)(SystemCoreClock * (f32)(1e-6)))

#define MCU_GET_SIGNATURE()     (DBGMCU->IDCODE & 0x00000FFF)
#define MCU_GET_REVISION()      ((DBGMCU->IDCODE >> 16) & 0x0000FFFF)

#define MCU_FLASH_SIZE_ADDRESS (0x1FFF7A22)
#define FLASH_SIZE_KB     (*(volatile u16 *)(MCU_FLASH_SIZE_ADDRESS))

#define MCU_ID_ADDRESS (0x1FFF7A10)
#define MCU_ID_U32     ((*(volatile u32 *)(MCU_ID_ADDRESS)

#if defined(DEBUG_BUILD)
  #define BP() \
  do \
  { \
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) \
    { \
      __asm("bkpt 1"); \
    } \
  } while (0)
#else
  #define BP()
#endif

INTERNAL void
initialise_cpu_timer(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk;
}

INTERNAL u32
read_cpu_timer(void)
{
  return DWT->CYCCNT;
}

// not accurate, e.g. if 0.8 ticks when called, only 0.2ticks elapsed
INTERNAL void 
delay_us(u32 us)
{
  u64 start_cycles = read_cpu_timer();
  u64 cycles_to_delay = us * SYSCLK_MHZ();
  cycles_to_delay -= 12;
  while ((read_cpu_timer() - start_cycles) < cycles_to_delay);
}

typedef u32 CLOCK_SOURCE;
enum
{
	CLOCK_SOURCE_HSI,
	CLOCK_SOURCE_HSE,
	CLOCK_SOURCE_SYSCLK,
	CLOCK_SOURCE_PCLK1,
	CLOCK_SOURCE_PCLK2,
	CLOCK_SOURCE_HCLK
};

INTERNAL u32 
get_clock_speed(CLOCK_SOURCE clock_source) 
{
	u32 result = 0;

	switch (clock_source) 
  {
		default: break;
		case CLOCK_SOURCE_HSI:
    {
			result = HSI_VALUE;
    } break;
		case CLOCK_SOURCE_HSE:
    {
			result = HSE_VALUE;
    } break;
		case CLOCK_SOURCE_HCLK:
    {
			result = HAL_RCC_GetHCLKFreq();
    } break;
		case CLOCK_SOURCE_PCLK1:
    {
			result = HAL_RCC_GetPCLK1Freq();
    } break;
		case CLOCK_SOURCE_PCLK2:
    {
			result = HAL_RCC_GetPCLK2Freq();
    } break;
		case CLOCK_SOURCE_SYSCLK:
    {
			result = HAL_RCC_GetSysClockFreq();
    } break;
	}
	
	return result;
}

typedef u32 RESET_SOURCE;
enum 
{
	RESET_SOURCE_NONE,
	RESET_SOURCE_LOW_POWER,
	RESET_SOURCE_WWDG,
	RESET_SOURCE_IWDG,
	RESET_SOURCE_SOFTWARE,
	RESET_SOURCE_POR,
	RESET_SOURCE_NRST,
	RESET_SOURCE_BOR
};

INTERNAL RESET_SOURCE 
get_reset_source(uint8_t clear_flags) 
{
  RESET_SOURCE result = RESET_SOURCE_NONE;

	if (RCC->CSR & RCC_CSR_LPWRRSTF) 
  {
		result = RESET_SOURCE_LOW_POWER;
	} 
  else if (RCC->CSR & RCC_CSR_WWDGRSTF)
  {
		result = RESET_SOURCE_WWDG;
	} 
  else if (RCC->CSR & RCC_CSR_WDGRSTF)
  {
		result = RESET_SOURCE_IWDG;
	} 
  else if (RCC->CSR & RCC_CSR_SFTRSTF)
  {
		result = RESET_SOURCE_SOFTWARE;
	} 
  else if (RCC->CSR & RCC_CSR_PORRSTF)
  {
		result = RESET_SOURCE_POR;
	} 
  else if (RCC->CSR & RCC_CSR_BORRSTF)
  {
		result = RESET_SOURCE_BOR;
	} 
  else if (RCC->CSR & RCC_CSR_PADRSTF)
  {
		result = RESET_SOURCE_NRST;
	}
	
	if (clear_flags) 
  {
		RCC->CSR = RCC_CSR_RMVF;
	}
	
	return result;
}


INTERNAL void 
soft_reset(void) 
{
	NVIC_SystemReset();
  // blink_led()
}

INTERNAL void 
force_hard_fault(void)
{
	typedef void (*hff)(void);
	hff hf_func = 0;
	hf_func();
}


#define cli() nvic_globalirq_disable()
#define sei() nvic_globalirq_enable()

INTERNAL void 
qemu_arm_exit(void) 
{
  register u32 r0 __asm__("r0");
  r0 = 0x18;
  register u32 r1 __asm__("r1");
  r1 = 0x20026;
  __asm__ volatile("bkpt #0xAB");
}

#endif
