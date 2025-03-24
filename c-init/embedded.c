// NOTE(Ryan): like on-chip bootloader, typically sdk init first which would be FPU, IVT location etc.
system_init()
{
   nvic_priority();
   init_systick();

   rcc_hse_pll_clock_source_init();
   rcc_apb_clock_init();
   rcc_periph_clock_init();


    NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));

   // gpio clock and pins -> uart clock -> nvic priority and interrupt enable
}

// each driver should be non-blocking, logging, console commands for testing, debugging (dump state) and measurement readings (start/stop)

// IMPORTANT: embedded 24/7 devices, so logging and defensive programming critical for robustness 
// detect (watchdog, exception) -> collect (log, error counter/registers) -> handle (automatic/manual restart; make decisions to store resumption state in flash)

// TODO(Ryan): red-jellies code for software debouncing, i.e. hold for 2seconds

#if TEST_BUILD
int testable_main(void)
#else
int main(void)
#endif
{
  if (system_init())
    while (1);

  // check_reset_reboot_reason();
  // enable_hardware_watchdog();

  // init peripherals
  debug_led_gpio_init();
  // used as a heartbeat, interrupt/error indicator etc.

  // Processing loop
	new_tick = HAL_GetTick();
	old_tick = new_tick;

  while (FOREVER) {
  	cur_tick = HAL_GetTick();
  	// check more than 1ms elapsed since previous loop iteration
  	if (cur_tick != new_tick) {
  	  old_tick = new_tick;
  	  new_tick = cur_tick;

  	  // as cumulative ticks, if running longer than 52days, 32bit overflow will occur
  	  if (new_tick < old_tick) {
  	    ticks = new_tick + (0xffffffff - old_tick);
  	  } else {
  	    ticks = new_tick - old_tick;
  	  }
  	} else {
  	  // ticks holds the amount of ms since last loop
  	  ticks = 0;
  	}



  	// update heartbeat led
  	heartbeat_ticks += ticks;
  	if (heartbeat_ticks >= heartbeat_tick_threshold) {
      gpio_user_led_green->toggle();
      heartbeat_ticks -= heartbeat_tick_threshold;
  	}

  }

}

enum PING_PONG_STATE {
  UNUSED, READ_COMPLETED
}
struct PingPongBuffer {
  volatile PING_PONG_STATE state;
  String8 buf;
  volatile u64 read_size; 
}
struct TemperatureState {
  usart_typdef uart_base;
  PingPongBuffer buffers[2];
  b32 error;
} 

void temperature_interrupt() {
    while (buf->state != ABS_UNUSED) { thread_yield(); }
    buf->state = READ_COMPLETED;
}

init() {
  TemperatureState ts = {};
  ts.buffers[0] = str8_allocate();
  ts.buffers[1] = str8_allocate();

  start_interrupt();

  u32 buf_i = 0;
  while (true) {
    PingPongBuffer *b = ts.buffers[buf_i++ & 1];
    while (b->state != READ_COMPLETED); 
    // process
    b->state = UNUSED; 
  }
}