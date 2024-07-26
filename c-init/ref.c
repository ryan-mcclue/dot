// SPDX-License-Identifier: zlib-acknowledgement

/**** Lists ****/
// get middle with slow and fast->next pointers
// dynamic list to array

// CPU retrieval for order not important
*opl = first + count;
for(Entity *e = first; e < opl; e += 1) if (e->is_active)
// dynamic alloc 
if (first_free == NULL) r = MEM_PUSH();
else
{
  r = first_free;
  r->active = true;
  first_free = first_free->next;
}
// dynamic release
entity->next = first_free;
entity->active = false;
first_free = entity;
// static alloc
if (first_free_id != NULL) r = SLL_QUEUE_POP()
// static release
SLL_QUEUE_PUSH(entity)
// chunked list
struct NodeChunk
{
  NodeChunk *next;
  Node *base;
  u32 count;
  u32 cap;
};
struct NodeChunkList
{
  NodeChunk *first, *last;
  u32 count;
  u32 cap;
};
/**** Errors ****/
struct Node { Node *next; };
GLOBAL Node global_nil_node = {&global_nil_node};
nil_node(void) { return &global_nil_node; }
is_nil_node(n) { return (n == NULL || n == &global_nil_node) }
for (*n = first; !is_nil_node(n); n = n->next)

struct Result {
  Data *d;
  ErrList errors;
}
struct ErrList {
  Err *first, *last;
  u32 count;
  enum max_err_kind;
}
struct Err { data, kind, string; }
Result r = ZERO_STRUCT;
r.d = nil;
if (err->kind > max_error_kind) push_err_list();

enum err_kind { NOTHING, NOTE, WARN, FATAL }
if (err_list.max_err_kind == NOTHING)

err_list_concat(r.errors, r2.errors);
// no code bifurcation
for (Err *e = errors.first; e != NULL; e = e->next)

/**** Macro Codegen (compile time execution) ****/
// we are removing the actual function calls with just counting
// may have to redefine types for it to work
#if COUNT_FUNCTION_CALLS
#define _mm_add_ps(a, b) ++Counts.mm_add_ps; a; (a is only thing want to count)
#endif

#define MD_EachNode(it, first) MD_Node *it = (first); !MD_NodeIsNil(it); it = it->next

// Enum print
switch (e)
{
#define CASE(n) case E_TYPE_##n: puts("E_" #n); break
  CASE(NEWLINE);
#undef CASE
}
// List
#define X_LIST \
  X(plug_init, void, Plug*) \
  X(plug_update, void, Plug*) \
#define X(name, return_type, ...) \
	typedef return_type (* name##_t)(__VA_ARGS__);
X_LIST
#undef X
// Meta-programming
#include "generated/samples.meta.[hc]" // include generated code?
if (TweakB32(draw_red_circle, 1)) {} // mark code for generation?
// this will reference a global variable, tweak_draw_red_circle
// this will generate a table of these variables, e.g. {"draw_red_circle", value, value_ptr}
// for (u32 i = 0; i < ARRAY_COUNT(tweak_b32_info); i += 1) {
//   slider(tweak->value_ptr, tweak->name)
// }
// EmbedFile(var_data, "filename")
// @TweakStruct()

// Indentation
MD_FUNCTION void
MD_DebugDumpFromNode(MD_Arena *arena, MD_String8List *out, MD_Node *node,
                     int indent, MD_String8 indent_string, MD_GenerateFlags flags)
{
#define MD_PrintIndent(_indent_level) do\
{\
for(int i = 0; i < (_indent_level); i += 1)\
{\
MD_S8ListPush(arena, out, indent_string);\
}\
}while(0)

/**** Hashmap ****/
struct NodeSlot { first, last; };
NodeSlot *map = PUSH_ARRAY(arena, NodeSlot, 256);
u64 i = hash_from_string(key) % 256;
NodeSlot *s = &map[i];
DLL_QUEUE_PUSH(s->first, s->last, node);

/**** Struct Layout ****/
// if literal combinations, use giant struct
// when reading from memory, think about will I evict something from cache line? 
// so, if have many objects of same type, just make struct smaller?
// for structs, as traversing from start of member list, advance by alignment of member (with final alignment of struct being that of largest member)
// 1. Remove unecessary values:
//   * can they be lazily evaluated? i.e. calculate over memoize
//   * pointers represented with indexes
//   * SINGLE-TYPE:
//      - boolean into enum tag, or out-of-band, e.g: separate into alive_monsters, dead_monsters
//      - Sparse data in hash maps, e.g: field in struct mostly empty, so separate into hashmap with original struct as key
//   * MULTIPLE-TYPES:
//     - fields inferred from enum encoding with external struct data, e.g. one type has disproportionate number of fields to the other
//       (if not, union of structs fine)
// 2. Remove unecessary padding with struct of arrays

/**** Sanitisation ****/
swap, max/min, noz, nil, logging

/**** Trees ****/
struct Node
{
  Node *next, *prev;
  Node *first, *last;
  Node *parent;
  Node *hash_next, *hash_prev;
};

// traversal

// recursive to iterative
trees if don't care loop with recursion
however, loop is faster and more reliable
https://www.youtube.com/watch?v=QkuNmL7tz08 

/**** Lexing ****/


/**** Parsing ****/
struct Node { Type t; String8 v; Node *first_child; Node *next; }

void parse_nested()
{
  Node *first_child;
  while (parsing_nested)
  {
    Node *child = parse_token(next_token);
    PUSH(first_child, child);
  }
  return first_child;
}

void parse_token()
{
  Node *first_child;
  if (nested_token) first_child = parse_nested();
  result->value = v;
  result->first_child = first_child;
}



/**** Bitwise ****/
// Long bitmask; property array

// Nested index
u32 i = (is_fullscreen << 1 | are_hovering);

// For < 9, array of unique characters interpreted as a u64
u64 table = *(u64 *)unique_characters;
// Unique character indices fit in 3 bits; index = (index << 3) | indexes[i];

// 32 apart, so mask off 32bit 
v = (string & ~0x10);
return (v >= 65 && v <= 90);


/**** Arenas ****/
Arena *state_arena;
Arena *entity_arena;
Arena *name_arena;

/**** SIMD ****/
// ensure buffer is of appropriate lane multiple, i.e. align buffer

// simd optimisation is about 'preamble', i.e how to reorganise data 
// 1. convert variables used into wide equivalent (a struct would have all members be individual variables)
//    convert any constants to wide equivalent (includes function parameters)
//    inline and convert to scalar math, e.g. V2 p; f32 x = ; f32 y = ;
//
// 2. loads:
//
//    writes:
//
//    may be necessary for loads/writes:
//    unpacking: __m128 x = _mm_set_ps(x + 3, x + 2, x + 1, x + 0);
//    so, have rgba | rgba | rgba | rgba
//    wrnt, rrrr | rrrr | rrrr | rrrr
//    r = _mm_and_ps(_mm_srli_epi(original_dest, 24), mask_ff)
//
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

// addps

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
#define LANE_F32_ADD(a, b) _mm_add_ps(a, b)


/**** Threading ****/
// Background
struct Ring {
 memory_index size;
 u8 *base;
 memory_index write_pos;
 memory_index read_pos;
 thread_mutex mutex;
 thread_cv cv;
};
item *consume(void) {
  data_item_t *item = NULL;
  LOCK_SCOPE(ring.lock) while (true)
  {
    if (ring.write_pos != ring.read_pos)
    {
      item = ring.base[ring.read_pos];
      ring.read_pos = (ring.read_pos + 1) % ring.size;
      break;
    }
    cv_wait(ring.cv, ring.lock);
  }
  cv_broadcast(ring.cv, ring.lock);

  return item;
}
void produce(item) {
  LOCK_SCOPE(ring.lock) while (true)
  {
    if ((ring.write_pos + 1) % ring.size != ring.read_pos)
    {
      ring.base[ring.write_pos] = item;
      ring.write_pos = (ring.write_pos + 1) % ring.size;
      break;
    }
    cv_wait(ring.cv, ring.lock);
  }
  cv_broadcast(ring.cv, ring.lock);
}
// Worker
struct WorkOrder
{
  min, max, sum;
};
struct WorkQueue
{
  u32 work_order_count;
  WorkOrder *work_orders;
  volatile u64 next_work_order_index;
  volatile u64 retired_count;
};
init()
{
  work_queue.work_order_count = num_cores;
  size = num_entries / num_cores;

  for (u32 i = 0; i < total_sum; ++i)
  {
    u32 min = i * size;
    u32 max = min + size;
    work_order->min = min;
    work_order->max = max;
  }

  for (u32 core_index = 1; core_index < core_count; ++core_index)
  {
    create_thread(&work_queue);
  }

  while (work_queue.tiles_retired_count < total_tile_count)
  {
    render_tile(&work_queue);
  }
}
thread_proc(void *arg)
{
  WorkQueue *work_queue = (WorkQueue *)arg;
  while (func(work_queue)) {}
  return 0;
}
func(WorkQueue *q)
{
  u64 work_order_index = locked_add_and_return_previous_value(&queue->next_work_order_index, 1);
  if (work_order_index >= queue->work_order_count)
  {
    return false;
  }

  WorkOrder *order = queue->work_orders + work_order_index;

  u32 sum = 0;
  for (u32 i = order->min; i < order->max; i += 1)
  {
    sum += q->nums[i]; 
  }

  locked_add_and_return_previous_value(&queue->retired_count, 1);
  return true;
}
/**** Drawing ****/
// General 
offset = font.size / 40.0f;
draw_panel(panel_region);
w = panel_region.h * 0.2f;
padding = w * 0.1f;

item_default = vec4_f32(0.3f, 0.8f, 0.9f, 1.0f);
item_hover = vec4_f32_whiten(item_default, 0.2f);
item_active = vec4_f32_whiten(item_default, 0.6f);

if (mouse_over_rect(item)) if (mouse_down()) draw_rect();
// Scrollbar
scrollable_h = num_items * item_h; 
t = scrollable_h / r.h;
scroll_h = r.h * t;
off -= dampening_v * dt;
if (off < 0) off = 0;
if (off > r.h - scroll_h) off = r.h - scroll_h;
item.y = item_h * r.y - off;
if (scrollable_h > r.y)
{
  q = off / scrollable_h;
  rect(x, r.h * q, w, scroll_h);
}
// Slider
slider_x, slider_cen_y;
val = slider_x + (mouse_x - slider_x);
CLAMP(slider_x, val, slider_x + slider_w - grip_w); 
// Pan
x += (mouse_x - pan_x); pan_x = mouse_x;
// Zoom
mouse_x / scale
item_width * scale
// Checkbox/radio
b32 checked = checkbox("text");
if (checked) draw_inner();
draw_text();
// Tabs
int i = radio("text1", "text2");
panel();
// Distance
length_sq(mouse - origin);
// Circle
step = TAU32 / ARRAY_COUNT(items);
p = origin + r * v2_arm(angle);
// Gradient
c = rgba(r, g, b, t)
// Color Wheel
rgb_to_hsv(); 
// Animation
cur += (target - cur) * (rate * dt);

expo_in = pow(2.f, -50.f * dt);
expo_out = 1 - pow(2.f, -50.f * dt); // (slow down)
cur += ((f32)!!is_active - cur) * expo_out; 

seconds = 1.0f;
t = delta / seconds;
t = sqrt(abs(t));
t = 1 - s; // (ease_out)

// interpolate range endpoints as well as range

t = cos_f32((ms - last_pressed) * dt);
t *= t;
t = 0.4f + 0.58f * t;

// Smooth movement
v *= 0.9f;
v += MouseWheel() * item_width * dt;
p += v;


/**** Math ****/
// Momentum
a += (+-)1.0f - friction * v;
v += a * dt;
p += 0.5f * a * dt * dt + v * dt;
// Angles
x = cos(a);
y = sin(a);
a = atan2(x, y);
// Logarithmic
for (f32 f = 20.0f; f < nyquist; f *= 1.06f)
u32 num_bars = F32_LOG(step, nyquist / 20.0f) + 1;

if (state->debug_overlay)

// Load small/medium/large font variants

