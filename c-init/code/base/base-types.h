// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_TYPES_H)
#define BASE_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>

typedef int8_t s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef u8 b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;
typedef float f32;
typedef double f64;
typedef size_t memory_index;


#define ASC_RED "\x1B[91m"
#define ASC_GREEN "\x1B[92m"
#define ASC_YELLOW "\x1B[93m"
#define ASC_BLUE "\x1B[94m"
#define ASC_MAGENTA "\x1B[95m"
#define ASC_CYAN "\x1B[96m"
#define ASC_WHITE "\x1B[97m"
#define ASC_CLEAR "\033[0m"

// NOTE(Ryan): PCLinting markup
// IMPORTANT(Ryan): Ignore integer constant used as NULL pointer
//#define ZERO_STRUCT_INIT /*lint -e9080*/ {0} /*lint +e9080*/
// IMPORTANT(Ryan): Ignore having no side-effects
//lint -esym(522 523, DEBUGGER_BREAK)

// C++ and C complex differently
#if LANG_CPP
  #include <complex>
  typedef std::complex<float> f32z;
  #define f32z_I f32z(0.0f, 1.0f)
  #define f32z_real(z) z.real()
  #define f32z_imaginary(z) z.imag()
  #define f32z_exp(z) std::exp(z)
  #define f32z_mag(z) std::abs(z)
#else
  #include <complex.h>
  typedef float complex f32z;
  #define f32z_I I
  #define f32z_real(z) crealf(z)
  #define f32z_imaginary(z) cimagf(z)
  #define f32z_exp(z) cexpf(z)
  #define f32z_mag(z) cabsf(z)
#endif


#include <stdbool.h>

#define GLOBAL static
#define LOCAL_PERSIST static
#if TEST_BUILD
 #define INTERNAL
#else
 #define INTERNAL static
#endif

#define GLOBAL_CONST PROGMEM

// NOTE(Ryan): For finishing superloop in test build  
#if TEST_BUILD
  GLOBAL u32 global_forever_counter = 1;
  #define FOREVER (global_forever_counter--) 
#else
  #define FOREVER 1
#endif

#if defined(SIMULATOR_BUILD) || TEST_BUILD
  // IMPORTANT(Ryan): For a function to be mocked/wrapped, it must be in a separate translation unit
  // For unity build, only function declaration can be present
  #define WANT_MOCKS 1
#endif

#define S8_MIN  ((s8)0x80)
#define S16_MIN ((s16)0x8000)
#define S32_MIN ((s32)0x80000000)
#define S64_MIN ((s64)0x8000000000000000ll)
#define SI8_MAX ((s8)0x7f) 
#define S16_MAX ((s16)0x7fff)
#define S32_MAX ((s32)0x7fffffff)
#define S64_MAX ((s64)0x7fffffffffffffffll)
#define U8_MAX  ((u8)0xff)
#define U16_MAX ((u16)0xffff)
#define U32_MAX ((u32)0xffffffff)
#define U64_MAX ((u64)0xffffffffffffffffllu)
// NOTE(Ryan): GCC will have the enum size accomodate the largest member
#define ENUM_U32_SIZE U32_MAX

// NOTE(Ryan): IEEE float 7 decimal places, double 15 decimal places
#define F32_MACHINE_EPSILON 1.1920929e-7f
#define F32_PI 3.1415926f // F32_ATAN2(1, 1) * 4, vector(1, 1) of 45°
#define F32_PI_DIV_180 0.0174532f
#define F32_180_DIV_PI 57.2957795f
// NOTE(Ryan): radian is arc length distance of 1 radius. 2pi/tau radians for circumference of circle. 
#define F32_TAU 6.2831853f
#define F32_HALF_PI 1.5707963f
#define F32_E 2.7182818f
#define F32_GOLD_BIG 1.6180339f
#define F32_GOLD_SMALL 0.6180339f
#define F64_MACHINE_EPSILON 2.220446049250313e-16
#define F64_PI 3.141592653589793
#define F64_PI_DIV_180 0.017453292519943
#define F64_180_DIV_PI 57.295779513082320
#define F64_TAU 6.283185307179586
#define F64_E 2.718281828459045
#define F64_GOLD_BIG 1.618033988749894
#define F64_GOLD_SMALL 0.618033988749894

// NOTE(Ryan): Taken from https://docs.oracle.com/cd/E19205-01/819-5265/bjbeh/index.html
INTERNAL f32
f32_inf(void)
{
  u32 temp = 0x7f800000;
  return *(f32 *)(&temp);
}

INTERNAL f32
f32_neg_inf(void)
{
  u32 temp = 0xff800000;
  return *(f32 *)(&temp);
}

INTERNAL f32
f32_noz(f32 a, f32 b)
{
  f32 result = 0.0f;

  if (b > F32_MACHINE_EPSILON)
  {
    result = a / b;  
  }

  return result;
}

INTERNAL f64
f64_inf(void)
{
  u64 temp = 0x7ff0000000000000;
  return *(f64 *)(&temp);
}

INTERNAL f64
f64_neg_inf(void)
{
  u64 temp = 0xfff0000000000000;
  return *(f64 *)(&temp);
}


INTERNAL f32 f32_abs(f32 f) { return f < 0.0f ? -f : +f; }
INTERNAL bool f32_eq(f32 a, f32 b) { return f32_abs(a - b) <= F32_MACHINE_EPSILON; }
INTERNAL f64 f64_abs(f64 f) { return f < 0.0 ? -f : +f; }
INTERNAL bool f64_eq(f64 a, f64 b) { return f64_abs(a - b) <= F64_MACHINE_EPSILON; }

typedef struct Handle Handle;
struct Handle
{ 
  void *addr;
  u64 gen;
};

typedef struct SourceLoc SourceLoc;
struct SourceLoc
{
  const char *file_name;
  const char *func_name;
  u64 line_num;
};
#define SOURCE_LOC { __FILE__, __func__, __LINE__ }

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

// IMPORTANT(Ryan): Cannot paste token delimiters like '.', '!' etc. so cannot do 'a. ## b'
#define PASTE_(a, b) a##b
#define PASTE(a, b) PASTE_(a, b)

#define PAD(n) char PASTE(pad, __LINE__)[n]

#define UNIQUE_NAME(name) PASTE(name, __LINE__)

#define DEFER_LOOP(begin, end) \
  for (int UNIQUE_NAME(var) = (begin, 0); \
       UNIQUE_NAME(var) == 0; \
       UNIQUE_NAME(var) += 1, end)
#define DEFER_LOOP_CHECKED(begin, end) \
  for (int UNIQUE_NAME(var) = 2 * !(begin); \
       (UNIQUE_NAME(var) == 2 ? ((end), 0) : !UNIQUE_NAME(var)); \
       UNIQUE_NAME(var) += 1, (end))
#define SCOPED(end) \
  for (int UNIQUE_NAME(var) = 0; \
       UNIQUE_NAME(var) == 0; \
       UNIQUE_NAME(var) += 1, end)

#define SWAP(t, a, b) do { t PASTE(temp__, __LINE__) = a; a = b; b = PASTE(temp__, __LINE__); } while(0)

#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))

#define INT_FROM_PTR(p) ((uintptr_t)((char *)p - (char *)0))
#define PTR_FROM_INT(n) ((void *)((char *)0 + (n)))

#define ABSTRACT_MEMBER(s, member) (((s *)0)->member)
#define OFFSET_OF_MEMBER(s, member) INT_FROM_PTR(&ABSTRACT_MEMBER(s, member))
#define CAST_FROM_MEMBER(S,m,p) (S*)(((u8*)p) - OFFSET_OF_MEMBER(S,m))

#define SET_FLAG(field, flag) ((field) |= (flag))
#define REMOVE_FLAG(field, flag) ((field) &= ~(flag))
#define TOGGLE_FLAG(field, flag) ((field) ^= (flag))
#define HAS_FLAGS_ANY(field, flags) (!!((field) & (flags)))
#define HAS_FLAGS_ALL(field, flags) (((field) & (flags)) == (flags))

// TODO(Ryan): Property arrays to store boolean properties about elements in an array?
#define PropArrayAdd(arr, prop)       ((arr)[(prop) / (sizeof((arr)[0])*8)] |=  (((U64)1) << ((prop) % (sizeof((arr)[0])*8))))
#define PropArrayRemove(arr, prop)    ((arr)[(prop) / (sizeof((arr)[0])*8)] &= ~(((U64)1) << ((prop) % (sizeof((arr)[0])*8))))
#define PropArrayGet(arr, prop)    (!!((arr)[(prop) / (sizeof((arr)[0])*8)] &   (((U64)1) << ((prop) % (sizeof((arr)[0])*8)))))

#define EACH_ENUM(type, it) type it = (type)0; it < type##_COUNT; it = (type)(it+1)
#define EACH_NONZERO_ENUM(type, it) type it = (type)1; it < type##_COUNT; it = (type)(it+1)


// TODO(Ryan): Date/time helpers?

#define SAT_ADD_U32(x, y) MIN((x + y), U32_MAX)

// type-limits, unused-value
#define SIGN_OF(x) ((x > 0) - (x < 0))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CLAMP(min,x,max) (((x)<(min))?(min):((max)<(x))?(max):(x))
#define CLAMP_TOP(a,b) MIN(a,b)
#define CLAMP_BOTTOM(a,b) MAX(a,b)

#define IS_POW2_ALIGNED(x, p) (((x) & ((p) - 1)) == 0)
#define IS_POW2(x) IS_POW2_ALIGNED(x, x) 
#define ALIGN_POW2_DOWN(x, p)       ((x) & -(p))
#define ALIGN_POW2_UP(x, p)       (-(-(x) & -(p)))
#define ALIGN_POW2_INCREASE(x, p)         (-(~(x) & -(p)))

#define STATIC_ASSERT(cond) typedef u8 UNIQUE_NAME(v) [(cond)?1:-1]
// NOTE(Ryan): Static array size checking:	STATIC_ASSERT((sizeof(a) / sizeof(*a)) >= 4)

#define THOUSAND(x) ((x)*1000LL)
#define MILLI_TO_SEC(x) ((x)*1000ULL)
#define MILLION(x)  ((x)*1000000LL)
#define MICRO_TO_SEC(x)  ((x)*1000000ULL)
#define BILLION(x)  ((x)*1000000000LL)
#define NANO_TO_SEC(x)  ((x)*1000000000ULL)
#define TRILLION(x) ((x)*1000000000000LL)
#define PICO_TO_SEC(x) ((x)*1000000000000ULL)

// NOTE(Ryan): Pre-compilation parses annotated structs and writes them to a header file
#define INTROSPECT(params)
// INTROSPECT(category:"something") typedef struct ParseThis
#define COUNTED_POINTER(count)
// COUNTED_POINTER(10) u32 *array

// NOTE(Ryan): Designated initialisers allow repetition and ZII
#define draw_rectdasdsadsad(r, ...) \
  draw_rect_((r), &(DrawRectParams){.color = {1,1,1,1}, __VA_ARGS__})

// NOTE(Ryan): Memory mapped address hierarchy typically 'bus + peripheral + register' 
#define MMIO_32(addr) (*(volatile u32 *)(addr))

#define RETURN_DEFER(val) do { result = (val); goto defer; } while (0)

#define INC_SATURATE_U8(x) ((x) = ((x) >= (U8_MAX) ? (U8_MAX) : (x + 1)))
#define INC_SATURATE_U16(x) ((x) = ((x) >= (U16_MAX) ? (U16_MAX) : (x + 1)))
#define INC_SATURATE_U32(x) ((x) = ((x) >= (U32_MAX) ? (U32_MAX) : (x + 1)))

#define PRINT_U32(var) printf(STRINGIFY(var) " = %" PRIu32 "\n", var)
#define PRINT_U64(var) printf(STRINGIFY(var) " = %" PRIu64 "\n", var)
#define PRINT_S32(var) printf(STRINGIFY(var) " = %" PRId32 "\n", var)
#define PRINT_S64(var) printf(STRINGIFY(var) " = %" PRId64 "\n", var)
#define PRINT_F32(var) printf(STRINGIFY(var) " = %f\n", var)
#define PRINT_F64(var) printf(STRINGIFY(var) " = %lf\n", var)
#define PRINT_STR8(var) printf(STRINGIFY(var) " = %.*s\n", str8_varg(var))

// IMPORTANT(Ryan): Better than templates as no complicated type checking or generation of little functions
#define __DLL_PUSH_FRONT(first, last, node, next, prev) \
(\
  ((first) == NULL) ? \
  (\
    ((first) = (last) = (node)), \
    ((node)->next = (node)->prev = NULL) \
  )\
  : \
  (\
    ((node)->prev = NULL), \
    ((node)->next = (first)), \
    ((first)->prev = (node)), \
    ((first) = (node)) \
  )\
)
#define DLL_PUSH_FRONT(first, last, node) \
  __DLL_PUSH_FRONT(first, last, node, next, prev)

// TODO(Ryan): Have macros for DLL_PUSH_FRONT_TREE_CHILD(), DLL_PUSH_FRONT_HASH_ITEM()
// the basic one assumes self contained
  
#define __DLL_PUSH_BACK(first, last, node, next, prev) \
(\
  ((first) == NULL) ? \
  (\
    ((first) = (last) = (node)), \
    ((node)->next = (node)->prev = NULL) \
  )\
  : \
  (\
    ((node)->prev = (last)), \
    ((node)->next = NULL), \
    ((last)->next = (node)), \
    ((last) = (node)) \
  )\
)
#define DLL_PUSH_BACK(first, last, node) \
  __DLL_PUSH_BACK(first, last, node, next, prev)

#define __DLL_INSERT(first, last, insert, node, next, prev) \
(\
  ((first) == NULL) ? \
  (\
    ((first) = (last) = (node)), \
    ((node)->next = (node)->prev = NULL) \
  )\
  : \
  (\
    ((insert) == NULL) ? \
    (\
      ((node)->prev = NULL, (node)->next = (first), \
        ((first) == NULL ? (NULL) : (first)->prev = (node)), (first) = (node)) \
      :\
      ((insert)->next == NULL ? (NULL) : \
       ((insert)->next->prev = (node), (node)->next = (insert)->next, (node)->prev = (insert), (insert)->next=(node)), \
         ((insert) == (last) ? (last) = (node) : (NULL)))\
    )\
  )\
)
#define DLL_INSERT(first, last, insert, node) \
  __DLL_INSERT(first, last, insert, node, next, prev)

#define __DLL_REMOVE(first, last, node, next, prev) \
(\
  ((node) == (first)) ? \
  (\
    ((first) == (last)) ? \
    (\
      ((first) = (last) = NULL) \
    )\
    : \
    (\
      ((first) = (first)->next), \
      ((first)->prev = NULL) \
    )\
  )\
  : \
  (\
    ((node) == (last)) ? \
    (\
      ((last) = (last)->prev), \
      ((last)->next = NULL) \
    )\
    : \
    (\
      ((node)->next->prev = (node)->prev), \
      ((node)->prev->next = (node)->next) \
    )\
  )\
)
#define DLL_REMOVE(first, last, node) \
  __DLL_REMOVE(first, last, node, next, prev) 

// TODO(Ryan): DLLInsert()

#define __SLL_QUEUE_PUSH(first, last, node, next) \
(\
  ((first) == NULL) ? \
   (\
    ((first) = (last) = (node)), \
    ((node)->next = NULL) \
   )\
  : \
  (\
    ((last)->next = (node)), \
    ((last) = (node)), \
    ((node)->next = NULL) \
  )\
)
#define SLL_QUEUE_PUSH(first, last, node) \
  __SLL_QUEUE_PUSH(first, last, node, next)

#define __SLL_QUEUE_POP(first, last, next) \
(\
  ((first) == (last)) ? \
    (\
     ((first) = (last) = NULL) \
    ) \
  : \
  (\
    ((first) = (first)->next) \
  )\
)
#define SLL_QUEUE_POP(first, last) \
  __SLL_QUEUE_POP(first, last, next)

#define __SLL_STACK_PUSH(first, node, next) \
(\
  ((node)->next = (first)), \
  ((first) = (node)) \
)
#define SLL_STACK_PUSH(first, node) \
  __SLL_STACK_PUSH(first, node, next)

#define __SLL_STACK_POP(first, next) \
(\
  ((first) != NULL) ? \
    (\
     ((first) = (first)->next) \
    )\
  : \
  (\
    (NULL) \
  )\
)
#define SLL_STACK_POP(first) \
  __SLL_STACK_POP(first, next)

#if 0
INTERNAL void
concat_list(List *a, List *b)
{
 if (a->last == NULL)
 {
  MEMORY_COPY_STRUCT(a, b);
 }
 else if (b->first != NULL)
 {
  a->last->next = b->first;
  a->last = b->last;
  a->count += b->count;
 }
 MEMORY_ZERO_STRUCT(b);
}
#endif

INTERNAL u64 
hash_ptr(void *p)
{
  u64 h = (u64)p;

  h = (h ^ (h >> 30)) * 0xbf58476d1ce4e5b9;
  h = (h ^ (h >> 27)) * 0x94d049bb133111eb;
  h = h ^ (h >> 31);

  return h;
}

#endif
