// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_PROFILER_H)
#define BASE_PROFILER_H

#if defined(PROFILER)
  #if defined(__GNUC_INSTRUMENTATION__)
    ProfileEphemeral ephemerals[4096];
    GLOBAL AddressIndexSlot g_address_index_slots;
    void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
    void __cyg_profile_func_enter(void *this_fn, void *call_site)
    {
      u32 ephemeral_slot = ptr_hash(this_fn) % ARRAY_COUNT(g_ephemerals);
      ProfileEphemeral *e = g_ephemerals[ephemeral_slot];
      ASSERT("Hash collision not encountered" && e->addr == this_fn);
      profile_block_start(ephemeral_slot);
    }   
    void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));
    void __cyg_profile_func_exit(void *this_fn, void *call_site)
    {
      u32 ephemeral_slot = ptr_hash(this_fn) % ARRAY_COUNT(g_ephemerals);
      ProfileEphemeral *e = g_ephemerals[ephemeral_slot];
      profile_block_end(e);
    }
 #endif

  typedef struct ProfileSlot ProfileSlot;
  struct ProfileSlot
  {
    u64 elapsed_no_children;
    u64 elapsed_children;
    u64 hit_count;
    u64 byte_count;
    const char *label;
  };
  
  typedef struct Profiler Profiler;
  struct Profiler
  {
    ProfileSlot slots[4096];
    u64 start;
    u64 end;
  };
  
  typedef struct ProfileEphemeral ProfileEphemeral;
  struct ProfileEphemeral
  {
    const char *label;
    u64 old_elapsed_children;
    u64 start;
    u32 parent_slot_index;
    u32 slot_index; 
  };
  
  GLOBAL Profiler global_profiler;
  GLOBAL u32 global_profiler_parent_slot_index;

  #define PROFILE_BLOCK(name) \
    for (struct {ProfileEphemeral e; u32 i;} UNIQUE_NAME(l) = {profile_block_start(name, __COUNTER__ + 1, 0), 0}; \
         UNIQUE_NAME(l).i == 0; \
         profile_block_end(&(UNIQUE_NAME(l)).e), UNIQUE_NAME(l).i++)
  #define PROFILE_FUNCTION() \
    PROFILE_BLOCK(__func__)
  #define PROFILE_BANDWIDTH(name, byte_count) \
    for (struct {ProfileEphemeral e; u32 i;} UNIQUE_NAME(l) = {profile_block_start(name, __COUNTER__ + 1, byte_count), 0}; \
         UNIQUE_NAME(l).i == 0; \
         profile_block_end(&(UNIQUE_NAME(l)).e), UNIQUE_NAME(l).i++)
  #define PROFILE_FUNCTION_BANDWIDTH(byte_count) \
    PROFILE_BANDWIDTH(__func__, byte_count)
  #define PROFILER_END_OF_COMPILATION_UNIT \
    STATIC_ASSERT(__COUNTER__ <= ARRAY_COUNT(global_profiler.slots));

  INTERNAL void
  profiler_init(void)
  {
    global_profiler.start = read_cpu_timer();
  }
  
  INTERNAL ProfileEphemeral
  profile_block_start(const char *label, u32 slot_index, u64 byte_count)
  {
    ProfileEphemeral ephemeral = ZERO_STRUCT;
    ephemeral.parent_slot_index = global_profiler_parent_slot_index;
    ephemeral.slot_index = slot_index;
    ephemeral.label = label;
  
    ProfileSlot *slot = global_profiler.slots + slot_index;
    ephemeral.old_elapsed_children = slot->elapsed_children;
    slot->byte_count += byte_count;
  
    global_profiler_parent_slot_index = slot_index;
    ephemeral.start = read_cpu_timer();
  
    return ephemeral;
  }
  
  INTERNAL u32
  profile_block_end(ProfileEphemeral *ephemeral)
  {
    u64 elapsed = read_cpu_timer() - ephemeral->start; 
    global_profiler_parent_slot_index = ephemeral->parent_slot_index;
  
    ProfileSlot *parent_slot = global_profiler.slots + ephemeral->parent_slot_index;
    ProfileSlot *slot = global_profiler.slots + ephemeral->slot_index;

    parent_slot->elapsed_no_children -= elapsed;
    slot->elapsed_no_children += elapsed;
    // handle recursion; just overwrite what children wrote
    slot->elapsed_children = ephemeral->old_elapsed_children + elapsed; 
    slot->hit_count++;
    slot->label = ephemeral->label;
  
    return 0;
  }
  
  INTERNAL void
  profiler_end_and_print(void)
  {
    global_profiler.end = read_cpu_timer();
    u64 total = global_profiler.end - global_profiler.start;
    u64 cpu_freq = linux_estimate_cpu_timer_freq();
    if (cpu_freq)
    {
      printf("\nTotal time: %0.4fms (%lu) (CPU freq %lu)\n", 1000.0 * (f64)total/(f64)cpu_freq, total, cpu_freq);
    }
  
    for (u32 i = 1; i < ARRAY_COUNT(global_profiler.slots); i += 1)
    {
      ProfileSlot *slot = global_profiler.slots + i;
      if (slot->hit_count == 0) break;
  
      f64 percent = 100.0 * ((f64)slot->elapsed_no_children / (f64)total);
      printf("  %s(%lu): %lu (%0.2f%%", slot->label, slot->hit_count, slot->elapsed_no_children, percent);
  
      if (slot->elapsed_children != slot->elapsed_no_children)
      {
        f64 percent_with_children = 100.0 * ((f64)slot->elapsed_children / (f64)total);
        printf(", %.2f%% w/children", percent_with_children); 
      }
      printf(")");

      if (slot->byte_count != 0)
      {
        f64 seconds = (f64)slot->elapsed_children/(f64)cpu_freq;
        f64 bps = (f64)slot->byte_count / seconds; 
        // TODO(Ryan): gb/s often what striving for? 0.3-0.5gb/s for modern CPUs?
        printf(" %.3fmb at %.2fgb/s", (f64)slot->byte_count / (f64)MB(1), bps / (f64)GB(1));
      }
      printf("\n");
    }
  }
#else
  #define PROFILER_END_OF_COMPILATION_UNIT
  #define PROFILE_FUNCTION()
  #define PROFILE_BLOCK(name)
  #define PROFILE_BANDWIDTH(name, byte_count)
  #define PROFILE_FUNCTION_BANDWIDTH(byte_count)

  typedef struct Profiler Profiler;
  struct Profiler
  {
    u64 start;
    u64 end;
  };
  
  GLOBAL Profiler global_profiler;
  
  INTERNAL void
  profiler_init(void)
  {
    global_profiler.start = read_cpu_timer();
  }

  INTERNAL void
  profiler_end_and_print(void)
  {
    global_profiler.end = read_cpu_timer();
    u64 total = global_profiler.end - global_profiler.start;
    u64 cpu_freq = linux_estimate_cpu_timer_freq();
    if (cpu_freq)
    {
      printf("\nTotal time: %0.4fms (CPU freq %lu)\n", 1000.0 * (f64)total/(f64)cpu_freq, cpu_freq);
    }
  }
#endif

#endif
