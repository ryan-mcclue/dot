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

// nice that intel defined intrinsics, so same across compilers

// TODO: do multithreading first

// simd optimisation is about 'preamble', i.e how to reorganise data 
// 1. convert variables used into wide equivalent (a struct would have all members be individual variables)
//    convert any constants to wide equivalent (includes function parameters)
//    inline and convert to scalar math, e.g. V2 p; f32 x = ; f32 y = ;
//    may be necessary for loads/writes:
//    unpacking: __m128 x = _mm_set_ps(x + 3, x + 2, x + 1, x + 0);
//    packing: for (i < 4) out[i] = LANE_ARR(x, i) << 4;
//    for (x = 0; x += 4)
//    {
//      r32 a[4]; // __m128 a;
//      r32 b[4];
//      b32 c[i];
//      r32 d[i];
//      for (i = 0; i < 4; i++)
//      {
//        LANE_F32_ARR(a, i) = ;
//        b[i] = ;
//      }
//
//      for (i = 0; i < 4; i++)
//      {
//        if (c[i])
//        {
//          d[i] = a[i] + b[i];
//        }
//      }
//    }
//  3. take out of for loop, and replace '+', '*', etc. with simd operations (so, shouldn't be seeing '+' operators)
//     easier to nest simd calls
//  a = _mm_mul_add(_mm_mul_ps(constant_wide, a), b);
//  4. handle packing (writing)
//  if want to combine smaller sizes, e.g. 8-bit values, shift and or
//  __m128i rounded_int = _mm_cvtps_epi32(); (round to nearest is default)
//  _mm_or_si128(rounded_int, _mm_slli_epi32(b, 8));
//
//   movdqa xmmword ptr [rdx-10h],xmm3
//   requires 16byte aligned memory (bottom 4 bits 1), so in debugger see ((rdx-10) & 15 == 0)
//  *(__m128i *)pixel = ;
//  so, to allow unaligned: _mm_storeu_si128((__m128i *)pixel, out)
//
//  want output to be say 8-bit rgba.
//  _mm_unpackhi_epi32() will interleave. 
//  __m128i br = _mm_unpacklo_epi32(_mm_castps_si128(b), r);
//  __m128i ag = _mm_unpacklo_epi32(a, g);
//  __m128i argb = _mm_unpack_lo_epi32(br, ag);
//  __m128i argb1 = _mm_unpack_hi_epi32(br, ag);
//  would arrange various interleaves of various sizes to get output order desired
//  4. handle unpacking (loading) 
//  5. handle conditionals
//  mask generation:
//  if () write_mask[i] = 0xffffffff;
//  write_mask = _mm_and_ps(_mm_cmpge_ps(U, Zero), _mm_cmple_ps(U, One));
//  original_dest = _mm_loadu_si128((__m128i *)pixel);
//  using write mask:
//  zero out places wanting to write to
//  a = _mm_andnot_si128(write_mask, original_dest)
//  zero out places we don't want to write
//  b = _mm_and_si128(write_mask, out)
//  fuse
//  c = _mm_or_si128(a, b)
//  _mm_storeu_si128(pixel, c)

// clamps are helpful to ensure inbounds and remove an if check
// if (cond) { op1; } else { op2; }
// we are always doing every aspect of if statement. 
// just selecting per lane
// a = op1;
// b = op2;
// write_mask(a, b)

// xmm registers hold any 32bit value
// movss xmm1, dword ptr []
// comiss xmm0, xmm7
// movaps xmm0, xmmword ptr []
#define LANE_F32_SS(a) _mm_set_ps1(a)
#define LANE_F32_PS(a, b, c, d) _mm_set_ps(a, b, c, d)

#define LANE_F32_SQRT(a) _mm_sqrt_ps(a)

#define LANE_F32_SQUARE(a) LANE_F32_MUL(a, a)

#define LANE_F64_SET(a) _mm_set_pd()

#define LANE_F32_ARRAY(l, i) ((* f32)(&l))[i]

#define LANE_F32_CLAMP01(a) _mm_min_ps(_mm_max_ps(a, zero_wide), one_wide)

// addps
#define LANE_F32_ADD(a, b) _mm_add_ps(a, b)

lane_f32 a = LANE_F32_SET(1.0f);
lane_f32 b = LANE_F32_SET(2.0f);
lane_f32 c = LANE_F32_ADD(a, b);

for (y < h)
  for (x < w)
{

}

for (y < h)
  for (x < w; x += 4)
    for (i < 4)
    {

    }
