// SPDX-License-Identifier: zlib-acknowledgement

/**** Lists ****/
// get middle with slow and fast->next pointers
// dynamic list to array

// CPU retrieval 
*opl = first + count;
for(Entity *e = first; e < opl; e += 1)
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
/**** Macro Codegen ****/
// NOTE(rjf): For-Loop Helpers
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
NodeSlot *search = hash_from_string(key) % 256;

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
t = r.h / scrollable_h;
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
// Cycle
t = cos_f32(ms - last_pressed);
t *= t;
t = 0.4f + 0.58f * t;
c = lerp(white, non_white, t);
// Gradient
c = rgba(r, g, b, t)
// Color Wheel
rgb_to_hsv(); 
// Animation
dynamic += (dst - val) * dt;
fast = 1 - pow(2.f, -50.f * dt);
static_hot_t = ((f32)!!is_hot - static_hot_t) * fast; 
lerp(start, end, static_hot_t);
/**** Math ****/
// Momentum
a += (+-)1.0f - friction * v;
v += a * dt;
p += 0.5f * a * dt * dt + v * dt;
// Artificial
a = MouseWheel() * h * 2.0f;
v += a * dt;
v *= d;
p += v;
// Angles
x = cos(a);
y = sin(a);
a = atan2(x, y);
// Logarithmic
for (f32 f = 20.0f; f < nyquist; f *= 1.06f)
u32 num_bars = F32_LOG(step, nyquist / 20.0f) + 1;

if (state->debug_overlay)

// Load small/medium/large font variants

