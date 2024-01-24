// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

typedef enum
{
  TESTER_STATE_UNINITIALISED = 0,
  TESTER_STATE_TESTING,
  TESTER_STATE_COMPLETED,
  TESTER_STATE_ERROR,
  TESTER_STATE_COUNT
} TESTER_STATE;

typedef struct RepetitionTester RepetitionTester;
struct RepetitionTester
{
  TESTER_STATE state;
  u64 time_accumulated_on_this_test;
  u64 bytes_accumulated_on_this_test;
  u64 start;
  u64 repeat_time;

  u64 cpu_timer_freq;
  u64 target_bytes_processed;

  u64 min_time;
  u64 max_time;
  u64 total_time;
  u64 test_count;
};

INTERNAL void
print_tester_time(RepetitionTester *tester, const char *label, u64 cpu_time, u64 byte_count)
{
  printf("%s: %.0f", label, (f64)cpu_time);
  f64 seconds = (f64)cpu_time / (f64)tester->cpu_timer_freq;
  printf(" (%fms)", 1000.0f*seconds);
  
  if (byte_count != 0)
  {
    f64 best_bandwidth = byte_count / (GB(1) * seconds);
    printf(" %fgb/s", best_bandwidth);
  }
}

INTERNAL void
print_tester_results(RepetitionTester *tester)
{
  print_tester_time(tester, "Min", (f64)tester->min_time, tester->target_bytes_processed);
  printf("\n");
  
  print_tester_time(tester, "Max", (f64)tester->max_time, tester->target_bytes_processed);
  printf("\n");
  
  if (tester->test_count)
  {
    print_tester_time(tester, "Avg", (f64)tester->total_time / (f64)tester->test_count, 
                      tester->target_bytes_processed);
    printf("\n");
  }
}

INTERNAL void
tester_error(RepetitionTester *tester, const char *msg)
{
  tester->state = TESTER_STATE_ERROR;
  WARN("%s", msg);
}

INTERNAL TESTER_STATE 
update_tester(RepetitionTester *tester)
{
  if (tester->state == TESTER_STATE_TESTING)
  {
    u64 current_time = read_cpu_timer();

    if (tester->bytes_accumulated_on_this_test != tester->target_bytes_processed)
    {
      tester_error(tester, "Repetition tester did not accumulate target bytes");
    }
    
    if (tester->state == TESTER_STATE_TESTING)
    {
      u32 elapsed_time = tester->time_accumulated_on_this_test;
      tester->test_count += 1;
      tester->total_time += tester->time_accumulated_on_this_test;
      if (tester->max_time < elapsed_time)
      {
        tester->max_time = elapsed_time;
      }
      if (tester->min_time > elapsed_time)
      {
        tester->min_time = elapsed_time;

        // NOTE(Ryan): So, repeat_time begins from most recent minimum
        tester->start = current_time;

        print_tester_time(tester, "New Min", tester->min_time, tester->bytes_accumulated_on_this_test);
        //printf("               \r");
        //fflush(stdout);
        printf("\n");
      }
      tester->time_accumulated_on_this_test = 0;
      tester->bytes_accumulated_on_this_test = 0;
    }

    if ((current_time - tester->start) > tester->repeat_time)
    {
      tester->state = TESTER_STATE_COMPLETED;
      //printf("                                                          \r");
      print_tester_results(tester);
    }
  }

  return tester->state;
}

INTERNAL u32
begin_test_time(RepetitionTester *tester)
{
  tester->time_accumulated_on_this_test -= read_cpu_timer();
  return 0;
}

INTERNAL u32
end_test_time(RepetitionTester *tester)
{
  tester->time_accumulated_on_this_test += read_cpu_timer();
  return 0;
}

#define TIME_TEST(tester) \
  for (u32 UNIQUE_NAME(v) = begin_test_time(tester); \
       UNIQUE_NAME(v) == 0; \
       end_test_time(tester), UNIQUE_NAME(v)++)

INTERNAL void
tester_count_bytes(RepetitionTester *tester, u64 byte_count)
{
  tester->bytes_accumulated_on_this_test += byte_count;
}
