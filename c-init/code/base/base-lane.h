// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

// NOTE(Ryan): Steam Hardware Survey as of March 2022: 
//  SSE4.1 (99.06%)
//  AVX (95.01%) 
// This gives us load instructions.
#if defined(__SSE4_1__) && defined(__AVX__)
#define LANE_WIDTH 4
struct LaneR32
{
  __m128 value;
};

struct LaneU32
{
  __m128i value;
};

#define LANE_R32_ADD(a, b) _mm_add_ps(a.value, b.value)
#define LANE_R32_MUL(a, b) _mm_add_ps(a.value, b.value)
#define LANE_R32_SUB(a, b) _mm_add_ps(a.value, b.value)
#define LANE_R32_DIV(a, b) _mm_add_ps(a.value, b.value)
#define LANE_R32_SLL(a, b) _mm_add_ps(a.value, b.value)
#define LANE_R32_SRL(a, b) _mm_add_ps(a.value, b.value)

#define LANE_R32_MIN(a, b) _mm_min_ps(a.value, b.value)
#define LANE_R32_MAX(a, b) _mm_max_ps(a.value, b.value)

#define LANE_R32_HADD(a, b) _mm_min_ps(a.value, b.value)

#elif defined(__AVX2__)

#define LANE_WIDTH 8
struct LaneR32
{
  __m256 value;
};

struct LaneU32
{
  __m256i value;
};

// TODO(Ryan): Implement AVX512
#else

#define LANE_WIDTH 1
struct LaneR32
{
  r32 value;
};

struct LaneU32
{
  u32 value;
};

#define LANE_R32_ADD_LANE_R32(a, b) (a.value + b.value)

#endif

struct lane_v3
{
  LaneR32 x;
  LaneR32 y;
  LaneR32 z;
}

INTERNAL LaneR32
operator+(LaneR32 a, LaneR32 b)
{
  LaneR32 result = {};

  result = LANE_R32_ADD_LANE_R32(a, b);

  return result;
}


INTERNAL LaneR32
operator-(LaneR32 a)
{
  LaneR32 result = {};

  result = LaneR32_from_r32(0) - a;

  return result;
}



INTERNAL LaneU32
and_not(LaneU32 a, LaneU32 b)
{
  LaneU32 result = {};

  result = _mm_andnot_si128(a.value, b.value);

  return result;
}

INTERNAL b32
mask_is_zeroed(LaneU32 a)
{
  int mask_value = _mm_movemask_epi8(a.value);

  return (mask_value == 0);
}

// SSSE3 has instruction for this
INTERNAL r64
horizontal_add(LaneR32 a)
{
  r32 *val = (r32 *)&a.value;

  r32 result = (r64)val[0] + (r64)val[1] + (r64)val[2] + (r64)val[3];

  return result;
}

INTERNAL lane_v3
lane_v3(LaneR32 x, LaneR32 y, LaneR32 z)
{
  lane_v3 result = {};

  result.x = x;
  result.y = y;
  result.z = z;

  return result;
}

// extract functions
INTERNAL V3
extract0(lane_v3 a)
{
  V3 result = {};

  result.x = (r32 *)&a.x[0];
  result.y = (r32 *)&a.y[0];
  result.z = (r32 *)&a.z[0];

  return result;
}


INTERNAL LaneU32
operator<(LaneR32 a, LaneR32 b)
{
  LaneU32 result = {};

  result = _mm_castps_si128(_mm_cmplt_ps(a.value, b.value));

  return result;
}

INTERNAL LaneU32
operator<<(LaneU32 a, u32 shift)
{
  LaneU32 result;

  result.value = _mm_slli_epi32(a.value, shift);

  return result;
}

INTERNAL LaneU32
operator>>(LaneU32 a, u32 shift)
{
  LaneU32 result;

  result.value = _mm_srli_epi32(a.value, shift);

  return result;
}

INTERNAL LaneU32
operator^(LaneU32 a, LaneU32 b)
{
  LaneU32 result;

  result = _mm_xor_si128(a.value, b.value);

  return result;
}

INTERNAL LaneR32
operator+(LaneR32 a, LaneR32 b)
{
  LaneR32 result;

  result = _mm_add_ps(a.value, b.value);

  return result;
}


INTERNAL LaneR32
operator/(LaneR32 a, r32 div)
{
  LaneR32 result;

  result.v = _mm_div_ps(a.result, _mm_set1_ps(div));

  return result;
}

INTERNAL LaneR32
LaneR32_from_LaneU32(LaneU32 a)
{
  LaneR32 result;

  result.value = _mm_cvtepi32_ps(a.value);

  return result;
}

INTERNAL LaneR32
LaneR32_from_u32(u32 replicate)
{
  LaneR32 result;

  result.value = _mm_set1_ps((r32)replicate);

  return result;
}

INTERNAL LaneR32
LaneR32_from_r32(r32 replicate)
{
  LaneR32 result;

  result.value = _mm_set1_ps(replicate);

  return result;
}

// state is seed
INTERNAL LaneU32
xor_shift_u32(u32 *random_series)
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	LaneU32 x = *random_series;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
  *random_series = x;

	return x;
}

INTERNAL LaneR32
LaneR32_from_u32(LaneU32 a)
{
  LaneR32 result = (LaneR32)a;

  return result;
}

INTERNAL LaneR32
random_unilateral(u32 *random_series)
{
  LaneR32 result = 0.0f;

  // rand() implements mutexes so each thread has to run serially, use rand_r()
  
  result = LaneR32_from_u32(xor_shift_u32(random_series)) / LaneR32_from_u32(U32_MAX);

  return result;
}

INTERNAL LaneR32
random_bilateral(u32 *random_series)
{
  LaneR32 result = 0.0f;

  result = -1.0f + 2.0f * random_unilateral(random_series);

  return result;
}

INTERNAL LaneR32
random_bilateral_lane(u32 *random_series)
{
  LaneR32 result = _mm_set_epi32(random_bilateral(random_series),
                                  random_bilateral(random_series),
                                  random_bilateral(random_series),
                                  random_bilateral(random_series));

  return result;
}

// TODO(Ryan): Do newer SIMD implement a not?
INTERNAL LaneU32
operator!=(LaneU32 a, LaneU32 b)
{
  LaneU32 result = {};

  result.value = _mm_xor_si128(_mm_cmpeq_si128(a.value, b.value), _mm_set1_epi32(0xFFFFFFFF));

  return result;
}

INTERNAL void
conditional_assign(LaneU32 *dest, LaneU32 mask, LaneU32 source)
{
  mask = (mask ? 0xffffffff : 0);

  *dest = (~mask & *dest) | (mask & source);
}

INTERNAL void
conditional_assign(LaneR32 *dest, LaneU32 mask, LaneR32 source)
{
  // only required for single lane
  mask = (mask ? 0xffffffff : 0);

  u32 result = (~mask & *(u32 *)dest) | (mask & *(u32 *)&source);
  *dest = *(r32 *)&result;
}

INTERNAL void
conditional_assign(lane_v3 *dest, LaneU32 mask, lane_v3 source)
{
  conditional_assign(&dest->x, mask, source.x);
  conditional_assign(&dest->y, mask, source.y);
  conditional_assign(&dest->z, mask, source.z);
}

INTERNAL LaneR32
max(LaneR32 a, LaneR32 b)
{
  return (a > b ? a : b);
}

INTERNAL b32
mask_is_zeroed(LaneU32 lane_mask)
{
  return (lane_mask == 0);
}

INTERNAL r32
horizontal_add(LaneR32 a)
{
  // IMPORTANT(Ryan): We are only doing this in one dimension for now
  return a;
}

INTERNAL V3
horizontal_add(lane_v3 a)
{
  V3 result = {};

  result.x = horizontal_add(a.x);
  result.y = horizontal_add(a.y);
  result.z = horizontal_add(a.z);

  return result;
}

INTERNAL LaneR32
random_bilateral_lane(u32 *random_series)
{
  LaneR32 result = random_bilateral(random_series);

  return result;
}

#else
#error Lane width must bet set to 1!
#endif

struct lane_v3
{
  LaneR32 x;
  LaneR32 y;
  LaneR32 z;
};

#if (LANE_WIDTH != 1)

INTERNAL lane_v3
lane_v3_from_v3(V3 replicate)
{
  lane_v3 result = {};

  result.x = LaneR32_from_r32(replicate.x);
  result.y = LaneR32_from_r32(replicate.y);
  result.z = LaneR32_from_r32(replicate.z);

  return result;
}

INTERNAL LaneU32 &
operator-=(LaneU32 &a, LaneU32 b)
{
  a = a - b;

  return a;
}

INTERNAL LaneU32 &
operator^=(LaneU32 &a, LaneU32 b)
{
  a.value = a ^ b;

  return a;
}

INTERNAL void
conditional_assign(LaneU32 *dest, LaneU32 mask, LaneU32 source)
{
  // IMPORTANT(Ryan): This works as masks obtained from simd comparison will be all 1s or all 0s
  *dest = and_not(mask, *dest) | (mask & source);
}

INTERNAL LaneR32
clamp01(LaneR32 a)
{
  LaneR32 result = {};

  result.value = LANE_R32_MIN(LANE_R32_MAX(a.value, LaneR32_from_r32(0.0f)), 1.0f);

  return result;
}

#endif
