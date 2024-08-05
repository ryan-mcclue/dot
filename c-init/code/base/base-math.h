// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_MATH_H)
#define BASE_MATH_H


// TODO(Ryan): sse_mathfun.h

#include <math.h>

#define SQUARE(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))

#define F32_CEIL(real32) ceilf(real32)
#define F32_ROUND(real32) roundf(real32)
#define F32_FLOOR(real32) floorf(real32)
#define F32_ROUND_U32(real32) (u32)roundf(real32)
#define F32_ROUND_S32(real32) (s32)roundf(real32)
#define F32_FLOOR_S32(real32) (s32)floorf(real32)
#define F32_FLOOR_U32(real32) (u32)floorf(real32)
#define F32_CEIL_U32(real32) (u32)ceilf(real32)
#define F32_CEIL_S32(real32) (s32)ceilf(real32) 
#define F32_DEG_TO_RAD(v) (F32_PI_DIV_180 * (v))
#define F32_RAD_TO_DEG(v) (F32_180_DIV_PI * (v))
#define F32_TURNS_TO_DEG(v) ((v) * 360.0f)
#define F32_TURNS_TO_RAD(v) ((v) * (F32_TAU))
#define F32_DEG_TO_TURNS(v) ((v) / 360.0f)
#define F32_RAD_TO_TURNS(v) ((v) / (F32_TAU))
#define F32_SQRT(x) sqrtf(x)
#define F32_SIN(x) sinf(x)
#define F32_COS(x) cosf(x)
#define F32_TAN(x) tanf(x)
#define F32_ATAN2(x, y) atan2f(x, y)
#define F32_LN(x) logf(x)
#define F32_LOG(b, a) (F32_LN(a) / F32_LN(b))
#define F32_MOD(x, y) fmodf(x, y)
#define F32_POW(x, y) powf(x, y)
#define F32_FMODF(x, y) fmodf(x, y) 
#define F32_ACOS(x) acosf(x)
#define F32_POWI(x, y) powf(x, y)

#define F64_SQRT(x) sqrt(x)
#define F64_SIN(x) sin(x) 
#define F64_COS(x) cos(x) 
#define F64_TAN(x) tan(x)
#define F64_LN(x) log(x)
#define F64_DEG_TO_RAD(v) (F64_PI_DIV_180 * (v))
#define F64_RAD_TO_DEG(v) (F64_180_DIV_PI * (v))
#define F64_TURNS_TO_DEG(v) ((v) * 360.0)
#define F64_TURNS_TO_RAD(v) ((v) * F64_TAU)
#define F64_DEG_TO_TURNS(v) ((v) / 360.0)
#define F64_RAD_TO_TURNS(v) ((v) / F64_TAU)

#if COMPILER_GCC && ARCH_X64
  INTERNAL u32 u32_count_bits_set(u32 val) { return (u32)__builtin_popcount(val); }
  INTERNAL u32 u32_count_leading_zeroes(u32 val) { return (u32)__builtin_clz(val); }
  INTERNAL u32 u32_count_trailing_zeroes(u32 val) { return (u32)__builtin_ctz(val); }
  INTERNAL u32 u32_get_parity(u32 val) { return (u32)__builtin_parity(val); }
  INTERNAL u16 u16_endianness_swap(u16 val) { return __builtin_bswap16(val); }
  INTERNAL u32 u32_endianness_swap(u32 val) { return __builtin_bswap32(val); }
  INTERNAL u64 u64_endianness_swap(u64 val) { return __builtin_bswap64(val); }
#else
  #warn Byte manipulation builtins not implemented
  INTERNAL u32 u32_count_bits_set(u32 val) { return 0; }
  INTERNAL u32 u32_count_leading_zeroes(u32 val) { return 0; }
  INTERNAL u32 u32_count_trailing_zeroes(u32 val) { return 0; }
  INTERNAL u32 u32_get_parity(u32 val) { return 0; }
  INTERNAL u16 u16_endianness_swap(u16 val) { return 0; }
  INTERNAL u32 u32_endianness_swap(u32 val) { return 0; }
  INTERNAL u64 u64_endianness_swap(u64 val) { return 0; }
#endif

INTERNAL f32
f32_sin_in(f32 t) 
{
  return F32_SIN((t - 1.0f) * F32_TAU) + 1.0f;
}

INTERNAL f32
f32_sin_out(f32 t)
{
  return F32_SIN(t * F32_TAU);
}

INTERNAL f32
f32_sin_in_out(f32 t)
{
  return 0.5f * (1.0f - F32_COS(t * F32_PI));
}

INTERNAL f32
f32_exp_out_fast(f32 t)
{
  return 1.0f - F32_POW(2.0f, -50.f * t);
}

INTERNAL f32
f32_exp_out_slow(f32 t)
{
  return 1.0f - F32_POW(2.0f, -20.f * t);
}

INTERNAL f32
f32_lerp(f32 a, f32 b, f32 t)
{
  f32 result = 0.0f;
  result = ((b - a) * t) + a; 

  return result;
}

INTERNAL memory_index
memory_index_round_to_nearest(memory_index val, memory_index near)
{
  memory_index result = val;

  result += near - 1;
  result -= result % near;
  
  return result;
}

// Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
INTERNAL u32 
u32_rand(u32 *seed)
{
  u32 x = *seed;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  *seed = x;

  return *seed;
}

INTERNAL f32 
f32_rand_unilateral(u32 *seed)
{
  u32 exponent = 127;
  u32 mantissa = u32_rand(seed) >> 9;
  u32 bits = (exponent << 23) | mantissa;
  f32 result = *(f32 *)&bits - 1.0f;

  return result;
}

INTERNAL f32 
f32_rand_bilateral(u32 *seed)
{
  return -1.0f + (2.0f * f32_rand_unilateral(seed));
}

INTERNAL u32 
u32_rand_range(u32 *seed, u32 range)
{
  u32 result = u32_rand(seed) % range;

  return result;
}

INTERNAL s32
s32_rand_range(u32 *seed, s32 min, s32 max)
{
  if (max < min)
  {
    max = min;
  }

  s32 result = min + (s32)u32_rand(seed) % (max - min + 1);

  return result;
}

INTERNAL f32 
f32_rand_range(u32 *seed, f32 min, f32 max)
{
  f32 range = f32_rand_unilateral(seed);
  f32 result = min + range * (max - min);

  return result;
}

INTERNAL f32
f32_norm(f32 start, f32 a, f32 end)
{
  f32 result = 0.0f;

  a = CLAMP(start, a, end);

  result = (end - a) / (end - start);

  return result;
}

INTERNAL f32
f32_map_to_range(f32 x0, f32 x1, f32 a, f32 y0, f32 y1)
{
  f32 result = 0.0f;

  f32 norm = f32_norm(x0, a, x1);

  result = y0 + norm * (y1 - y0);

  return result;
}

#if 0
#define VECTOR_SIZE(amount, type) \
  vector_size(amount * sizeof(type))

typedef r32 v2 __attribute__((VECTOR_SIZE(2, r32)));
typedef r32 v3 __attribute__((VECTOR_SIZE(3, r32)));
typedef r32 v4 __attribute__((VECTOR_SIZE(4, r32)));

typedef union V2
{
  v2 vec;
  struct
  {
    r32 x, y;
  };
  r32 e[2];
} V2;

typedef union V3
{
  v3 vec;
  struct
  {
    r32 x, y, z;
  };
  r32 e[3];
} V3;

typedef union V4
{
  v4 vec;
  struct
  {
    r32 x, y, z, w;
  };
  r32 e[4];
} V4;
#endif

// NOTE(Ryan): To allow for anonymous structs
IGNORE_WARNING_PEDANTIC()
typedef union Vec2F32 Vec2F32;
union Vec2F32
{
  struct
  {
    f32 x, y;
  };
  struct
  {
    f32 w, h;
  };

  f32 elements[2];
  f32 v[2];
};

typedef union Vec2S32 Vec2S32;
union Vec2S32
{
  struct
  {
    s32 x, y;
  };

  struct
  {
    s32 w, h;
  };

  s32 elements[2];
  s32 v[2];
};

typedef union Vec2U32 Vec2U32;
union Vec2U32
{
  struct
  {
    u32 x, y;
  };

  struct
  {
    u32 w, h;
  };

  u32 elements[2];
  u32 v[2];
};

typedef union Vec2S64 Vec2S64;
union Vec2S64
{
  struct
  {
    s64 x, y;
  };

  s64 elements[2];
  s64 v[2];
};

typedef union Vec3F32 Vec3F32;
union Vec3F32
{
  struct
  {
    f32 x, y, z;
  };

  struct
  {
    f32 r, g, b;
  };

  struct
  {
    Vec2F32 xy;
    f32 z1;
  };

  struct
  {
    f32 x1;
    Vec2F32 yz;
  };

  f32 elements[3];
  f32 v[3];
};

typedef union Vec3S32 Vec3S32;
union Vec3S32
{
  struct
  {
    s32 x, y, z;
  };

  struct
  {
    s32 r, g, b;
  };

  s32 elements[3];
  s32 v[3];
};

typedef union Vec3S64 Vec3S64;
union Vec3S64
{
  struct
  {
    s64 x, y, z;
  };

  struct
  {
    s64 r, g, b;
  };

  s64 elements[3];
  s64 v[3];
};


typedef union Vec4F32 Vec4F32;
union Vec4F32
{
  struct
  {
    f32 x, y, z, w;
  };

  struct
  {
    Vec2F32 xy, zw;
  };

  struct
  {
    Vec3F32 xyz;
    f32 w1;
  };

  struct
  {
    f32 x1;
    Vec3F32 yzw;
  };

  struct
  {
    f32 r, g, b, a;
  };

  struct
  {
    Vec3F32 rgb;
    f32 a1;
  };

  struct
  {
    f32 r1;
    Vec3F32 gba;
  };

  f32 elements[4];
  f32 v[4];
};

typedef union Vec4S32 Vec4S32;
union Vec4S32
{
  struct
  {
    s32 x, y, z, w;
  };

  struct
  {
    Vec2S32 xy, zw;
  };

  struct
  {
    Vec3S32 xyz;
    s32 w1;
  };

  struct
  {
    s32 x1;
    Vec3S32 yzw;
  };

  struct
  {
    s32 r, g, b, a;
  };

  struct
  {
    Vec3S32 rgb;
    s32 a1;
  };

  struct
  {
    s32 r1;
    Vec3S32 gba;
  };

  s32 elements[4];
  s32 v[4];
};

typedef union Vec4S64 Vec4S64;
union Vec4S64
{
  struct
  {
    s64 x, y, z, w;
  };

  struct
  {
    Vec2S64 xy, zw;
  };

  struct
  {
    Vec3S64 xyz;
    s64 w1;
  };

  struct
  {
    s64 x1;
    Vec3S64 yzw;
  };

  struct
  {
    s64 r, g, b, a;
  };

  struct
  {
    s64 r1;
    Vec3S64 gba;
  };

  struct
  {
    Vec3S64 rgb;
    s64 a1;
  };

  s64 elements[4];
  s64 v[4];
};
IGNORE_WARNING_POP()

#define vec2_f32_dup(a) vec2_f32((a), (a))
#define vec3_f32_dup(a) vec3_f32((a), (a), (a))

INTERNAL Vec2F32 vec2_f32(f32 x, f32 y) { return {x, y}; }
INTERNAL Vec2F32 vec2_f32_add(Vec2F32 a, Vec2F32 b) { return vec2_f32(a.x + b.x, a.y + b.y); }
INTERNAL Vec2F32 vec2_f32_sub(Vec2F32 a, Vec2F32 b) { return vec2_f32(a.x - b.x, a.y - b.y); }
INTERNAL Vec2F32 vec2_f32_neg(Vec2F32 a) { return vec2_f32(-a.x, -a.y); }
INTERNAL Vec2F32 vec2_f32_hadamard(Vec2F32 a, Vec2F32 b) { return vec2_f32(a.x * b.x, a.y * b.y); }
INTERNAL Vec2F32 vec2_f32_mul(Vec2F32 a, f32 b) { return vec2_f32(a.x * b, a.y * b); }
INTERNAL Vec2F32 vec2_f32_div(Vec2F32 a, Vec2F32 b) { return vec2_f32(a.x / b.x, a.y / b.y); }
INTERNAL Vec2F32 vec2_f32_arm(f32 angle) { return vec2_f32(F32_COS(angle), F32_SIN(angle)); }
INTERNAL Vec2F32 vec2_f32_perp(Vec2F32 a) { return vec2_f32(-a.y, a.x); }
INTERNAL f32 vec2_f32_angle(Vec2F32 a) { return F32_ATAN2(a.y, a.x); }
INTERNAL f32 vec2_f32_dot(Vec2F32 a, Vec2F32 b) { return (a.x * b.x + a.y * b.y); }
INTERNAL f32 vec2_f32_lengthsq(Vec2F32 v) { return vec2_f32_dot(v, v); }
INTERNAL f32 vec2_f32_length(Vec2F32 v) { return F32_SQRT(vec2_f32_lengthsq(v)); }
INTERNAL Vec2F32 vec2_f32_normalise(Vec2F32 v) { return vec2_f32_mul(v, 1.0f / vec2_f32_length(v)); }
INTERNAL Vec2F32 vec2_f32_lerp(Vec2F32 a, Vec2F32 b, f32 t) { return vec2_f32(a.x * (1 - t) + (b.x * t), a.y * (1 - t) + (b.y * t)); }

#if defined(LANG_CPP)
INTERNAL Vec2F32 operator*(f32 s, Vec2F32 a) { return vec2_f32_mul(a, s); }
INTERNAL Vec2F32 operator*(Vec2F32 a, f32 s) { return vec2_f32_mul(a, s); }
INTERNAL Vec2F32 & operator*=(Vec2F32 &a, f32 s) { a = a * s; return a; } 
INTERNAL Vec2F32 operator+(Vec2F32 a, Vec2F32 b) { return vec2_f32_add(a, b); }
INTERNAL Vec2F32 & operator+=(Vec2F32 &a, Vec2F32 b) { a = a + b; return a; }
INTERNAL Vec2F32 operator-(Vec2F32 a, Vec2F32 b) { return vec2_f32_sub(a, b); }
INTERNAL Vec2F32 & operator-=(Vec2F32 &a, Vec2F32 b) { a = a - b; return a; }
INTERNAL Vec2F32 operator-(Vec2F32 a) { return vec2_f32_neg(a); }
#endif
// TODO(Ryan): Add gcc vector extensions for C

INTERNAL Vec2F32
vec2_f32_reflect(Vec2F32 incident, Vec2F32 normal)
{
  Vec2F32 result = ZERO_STRUCT;

  Vec2F32 normal_unit = vec2_f32_normalise(normal);

  result = -2.0f * (vec2_f32_dot(incident, normal_unit)) * normal_unit;

  return result;
}

INTERNAL Vec3F32 vec3_f32(f32 x, f32 y, f32 z) { return {x, y, z}; }
INTERNAL Vec3F32 vec3_f32_add(Vec3F32 a, Vec3F32 b) { return vec3_f32(a.x + b.x, a.y + b.y, a.z + b.z); }
INTERNAL Vec3F32 vec3_f32_sub(Vec3F32 a, Vec3F32 b) { return vec3_f32(a.x - b.x, a.y - b.y, a.z - b.z); }
INTERNAL Vec3F32 vec3_f32_hadamard(Vec3F32 a, Vec3F32 b) { return vec3_f32(a.x * b.x, a.y * b.y, a.z * b.z); }
INTERNAL Vec3F32 vec3_f32_div(Vec3F32 a, Vec3F32 b) { return vec3_f32(a.x / b.x, a.y / b.y, a.z / b.z); }
INTERNAL Vec3F32 vec3_f32_mul(Vec3F32 a, f32 scale) { return vec3_f32(a.x * scale, a.y * scale, a.z * scale); }
INTERNAL f32 vec3_f32_dot(Vec3F32 a, Vec3F32 b) { return (a.x * b.x + a.y * b.y + a.z * b.z); }
INTERNAL f32 vec3_f32_lengthsq(Vec3F32 v) { return vec3_f32_dot(v, v); }
INTERNAL f32 vec3_f32_length(Vec3F32 v) { return F32_SQRT(vec3_f32_lengthsq(v)); }
INTERNAL Vec3F32 vec3_f32_normalise(Vec3F32 v) { return vec3_f32_mul(v, 1.0f / vec3_f32_length(v)); }
INTERNAL Vec3F32 vec3_f32_lerp(Vec3F32 a, Vec3F32 b, f32 t) { return vec3_f32(a.x * (1 - t) + (b.x * t), a.y * (1 - t) + (b.y * t), a.z * (1 - t) + (b.z * t)); }
INTERNAL Vec3F32 vec3_f32_cross(Vec3F32 a, Vec3F32 b) { return vec3_f32(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }


INTERNAL Vec4F32 vec4_f32(f32 x, f32 y, f32 z, f32 w) { return {x, y, z, w}; }
INTERNAL Vec4F32 vec4_f32_add(Vec4F32 a, Vec4F32 b) { return vec4_f32(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
INTERNAL Vec4F32 vec4_f32_sub(Vec4F32 a, Vec4F32 b) { return vec4_f32(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
INTERNAL Vec4F32 vec4_f32_hadamard(Vec4F32 a, Vec4F32 b) { return vec4_f32(a.x * b.x, a.y * b.y, a.z * b.z, a.z * b.z); }
INTERNAL Vec4F32 vec4_f32_div(Vec4F32 a, Vec4F32 b) { return vec4_f32(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
INTERNAL Vec4F32 vec4_f32_mul(Vec4F32 a, f32 scale) { return vec4_f32(a.x * scale, a.y * scale, a.z * scale, a.w * scale); }
INTERNAL Vec4F32 vec4_f32_neg(Vec4F32 a) { return vec4_f32(-a.x, -a.y, -a.z, -a.w); }
INTERNAL f32 vec4_f32_dot(Vec4F32 a, Vec4F32 b) { return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w); }
INTERNAL f32 vec4_f32_lengthsq(Vec4F32 v) { return vec4_f32_dot(v, v); }
INTERNAL f32 vec4_f32_length(Vec4F32 v) { return F32_SQRT(vec4_f32_lengthsq(v)); }
INTERNAL Vec4F32 vec4_f32_normalise(Vec4F32 v) { return vec4_f32_mul(v, 1.0f / vec4_f32_length(v)); }
INTERNAL Vec4F32 vec4_f32_lerp(Vec4F32 a, Vec4F32 b, f32 t) { return vec4_f32(a.x * (1 - t) + (b.x * t), a.y * (1 - t) + (b.y * t), a.z * (1 - t) + (b.z * t), a.w * (1 - t) + (b.w * t)); }

INTERNAL Vec4F32
vec4_f32_hex(u32 hex)
{
  Vec4F32 result = ZERO_STRUCT;

  result.r = (hex >> 24) / 255.0f;
  result.g = (hex >> 16 & 0xff) / 255.0f;
  result.b = (hex >> 8 & 0xff) / 255.0f;
  result.a = (hex & 0xff) / 255.0f;

  return result;
}

INTERNAL Vec4F32
vec4_f32_whiten(Vec4F32 colour, f32 amount)
{
  Vec4F32 result = ZERO_STRUCT;

  result.r = f32_lerp(colour.r, 1.0f, amount);
  result.g = f32_lerp(colour.g, 1.0f, amount);
  result.b = f32_lerp(colour.b, 1.0f, amount);

  result.a = 1.0f;

  return result;
}

INTERNAL Vec4F32
vec4_f32_darken(Vec4F32 colour, f32 amount)
{
  Vec4F32 result = ZERO_STRUCT;

  result.r = f32_lerp(colour.r, 0.0f, amount);
  result.g = f32_lerp(colour.g, 0.0f, amount);
  result.b = f32_lerp(colour.b, 0.0f, amount);

  result.a = 1.0f;

  return result;
}

INTERNAL Vec4F32 
vec4_f32_brightness(Vec4F32 color, f32 amount)
{
  Vec4F32 result = color;

  result.r *= amount;
  result.g *= amount;
  result.b *= amount;

  return result;
}

INTERNAL Vec4F32
vec4_f32_rgb_from_hsv(Vec3F32 hsv)
{
  Vec4F32 rgb = ZERO_STRUCT;
  rgb.a = 1.0f;
 
  if (hsv.y < F32_MACHINE_EPSILON)
  {
    rgb.r = rgb.g = rgb.b = hsv.z;
  }
  else
  {
    f32 h = hsv.x;
    f32 s = hsv.y;
    f32 v = hsv.z;
   
    if (h >= 1.0f)
    {
      h -= 10 * 1e-6f;
    }
    
    if (s >= 1.0f)
    {
      s -= 10 * 1e-6f;
    }
    
    if (v >= 1.0f)
    {
      v -= 10 * 1e-6f;
    }
    
    h = F32_MOD(h, 1.0f) / (60.0f/360.0f);
    int i = (int)h;
    f32 f = h - (f32)i;
    f32 p = v * (1.0f - s);
    f32 q = v * (1.0f - s * f);
    f32 t = v * (1.0f - s * (1.0f - f));
    
    switch (i)
    {
      case 0: { rgb.r = v; rgb.g = t; rgb.b = p; break; }
      case 1: { rgb.r = q; rgb.g = v; rgb.b = p; break; }
      case 2: { rgb.r = p; rgb.g = v; rgb.b = t; break; }
      case 3: { rgb.r = p; rgb.g = q; rgb.b = v; break; }
      case 4: { rgb.r = t; rgb.g = p; rgb.b = v; break; }
      case 5: { default: rgb.r = v; rgb.g = p; rgb.b = q; break; }
    }
  }
  
  return rgb;
}

#if defined(LANG_CPP)
INTERNAL Vec4F32 operator*(f32 s, Vec4F32 a) { return vec4_f32_mul(a, s); }
INTERNAL Vec4F32 operator*(Vec4F32 a, f32 s) { return vec4_f32_mul(a, s); }
INTERNAL Vec4F32 & operator*=(Vec4F32 &a, f32 s) { a = a * s; return a; } 
INTERNAL Vec4F32 operator+(Vec4F32 a, Vec4F32 b) { return vec4_f32_add(a, b); }
INTERNAL Vec4F32 & operator+=(Vec4F32 &a, Vec4F32 b) { a = a + b; return a; }
INTERNAL Vec4F32 operator-(Vec4F32 a, Vec4F32 b) { return vec4_f32_sub(a, b); }
INTERNAL Vec4F32 & operator-=(Vec4F32 &a, Vec4F32 b) { a = a - b; return a; } 
INTERNAL Vec4F32 operator-(Vec4F32 a) { return vec4_f32_neg(a); }
#endif
// TODO(Ryan): Add gcc vector extensions for C


INTERNAL Vec2U32 vec2_u32(u32 x, u32 y) { return {x, y}; }
INTERNAL Vec2S32 vec2_s32(s32 x, s32 y) { return {x, y}; }
INTERNAL Vec2S64 vec2_s64(s64 x, s64 y) { return {x, y}; }

INTERNAL u32
u32_pack_4x8(Vec4F32 val)
{
  u32 result = 0;

  result = (F32_ROUND_U32(val.x) << 24 |
            F32_ROUND_U32(val.y) << 16 & 0xFF0000 |
            F32_ROUND_U32(val.z) << 8 & 0xFF00 |
            F32_ROUND_U32(val.w) & 0xFF);

  return result;
}

IGNORE_WARNING_PEDANTIC()
typedef union RangeU32 RangeU32;
union RangeU32
{
  struct
  {
    u32 min, max;
  };
  u32 v[2];
};

INTERNAL RangeU32
range_u32(u32 min, u32 max)
{
  RangeU32 result = {min, max};
  if (result.max < result.min)
  {
    SWAP(u32, result.min, result.max);
  }
  return result;
}

INTERNAL RangeU32 range_u32_pad(RangeU32 r, u32 x) { return range_u32(r.min-x, r.max+x); }
INTERNAL u32 range_u32_centre(RangeU32 r) { return (r.min + r.max)/2; }
INTERNAL b32 range_u32_contains(RangeU32 r, u32 v) { return r.min <= v && v < r.max; }
INTERNAL u32 range_u32_dim(RangeU32 r) { return (r.max - r.min); }
INTERNAL RangeU32 range_u32_union(RangeU32 a, RangeU32 b) { return range_u32(MIN(a.min, b.min), MAX(a.max, b.max)); }
INTERNAL RangeU32 range_u32_intersection(RangeU32 a, RangeU32 b) { return range_u32(MAX(a.min, b.min), MIN(a.max, b.max)); }

typedef union RangeU64 RangeU64;
union RangeU64
{
  struct
  {
    u64 min, max;
  };
  u64 v[2];
};

INTERNAL RangeU64
range_u64(u64 min, u64 max)
{
  RangeU64 result = {min, max};
  if (result.max < result.min)
  {
    SWAP(u64, result.min, result.max);
  }
  return result;
}

INTERNAL RangeU64 range_u64_pad(RangeU64 r, u64 x) { return range_u64(r.min-x, r.max+x); }
INTERNAL u64 range_u64_centre(RangeU64 r) { return (r.min + r.max)/2; }
INTERNAL b64 range_u64_contains(RangeU64 r, u64 v) { return r.min <= v && v < r.max; }
INTERNAL u64 range_u64_dim(RangeU64 r) { return (r.max - r.min); }
INTERNAL RangeU64 range_u64_union(RangeU64 a, RangeU64 b) { return range_u64(MIN(a.min, b.min), MAX(a.max, b.max)); }
INTERNAL RangeU64 range_u64_intersection(RangeU64 a, RangeU64 b) { return range_u64(MAX(a.min, b.min), MIN(a.max, b.max)); }

IGNORE_WARNING_PEDANTIC()
typedef union RectF32 RectF32;
union RectF32
{
  struct
  {
    Vec2F32 pos, size;
  };
  struct
  {
    f32 x, y, w, h;
  };
  f32 e[4];
};
IGNORE_WARNING_POP()

INTERNAL RectF32 rect_f32(Vec2F32 pos, Vec2F32 size) { return {pos, size}; }

INTERNAL b32 
rect_f32_contains(RectF32 r, Vec2F32 v) 
{ 
  return (v.x >= r.x && v.x <= r.x+r.w) && (v.y >= r.y && v.y <= r.y+r.h);
}

INTERNAL RectF32 
rect_f32_shift(RectF32 r, Vec2F32 v) 
{ 
  r.x += v.x; 
  r.y += v.y; 

  return r; 
}

INTERNAL RectF32 
rect_f32_pad(RectF32 r, f32 x) 
{ 
  Vec2F32 min = vec2_f32_sub(r.pos, vec2_f32(x, x));
  Vec2F32 max = vec2_f32_add(vec2_f32_add(r.pos, r.size), vec2_f32(x, x));

  return rect_f32(min, max); 
}

INTERNAL Vec2F32 
rect_f32_centre(RectF32 r) 
{ 
  return vec2_f32((r.pos.x + r.size.x) * 0.5f, (r.pos.y + r.size.y) * 0.5f); 
}

INTERNAL Vec2F32 
rect_f32_dim(RectF32 r) 
{ 
  return vec2_f32(r.size.x, r.size.y); 
}

#if 0
GLOBAL Vec4F32 WHITE_COLOUR = vec4_f32(1.0f, 1.0f, 1.0f, 1.0f);
GLOBAL Vec4F32 OFFWHITE_COLOUR = vec4_f32_hex(0xfdf6e3);
GLOBAL Vec4F32 BLACK_COLOUR = vec4_f32(0.0f, 0.0f, 0.0f, 1.0f);
GLOBAL Vec4F32 OFFBLACK_COLOUR = vec4_f32_hex(0x002b36);
GLOBAL Vec4F32 LIGHTGREY_COLOUR = vec4_f32(0.93f, 0.91f, 0.88f, 1.0f);
GLOBAL Vec4F32 DARKGREY_COLOUR = vec4_f32(0.35f, 0.38f, 0.40f, 1.0f);
GLOBAL Vec4F32 YELLOW_COLOUR = vec4_f32(0.71f, 0.54f, 0.00f, 1.0f);
GLOBAL Vec4F32 ORANGE_COLOUR = vec4_f32(0.80f, 0.29f, 0.09f, 1.0f);
GLOBAL Vec4F32 RED_COLOUR = vec4_f32(0.86f, 0.20f, 0.18f, 1.0f);
GLOBAL Vec4F32 MAGENTA_COLOUR = vec4_f32(0.83f, 0.21f, 0.05f, 1.0f);
GLOBAL Vec4F32 VIOLET_COLOUR =vec4_f32(0.42f, 0.44f, 0.77f, 1.0f);
GLOBAL Vec4F32 BLUE_COLOUR = vec4_f32(0.15f, 0.55f, 0.82f, 1.0f);
GLOBAL Vec4F32 CYAN_COLOUR = vec4_f32(0.16f, 0.63f, 0.60f, 1.0f);
GLOBAL Vec4F32 GREEN_COLOUR = vec4_f32(0.52f, 0.60f, 0.00f, 1.0f);
#endif

#endif
