// SPDX-License-Identifier: zlib-acknowledgement

// TODO: graphs: https://hero.handmade.network/episode/code/day302/#2125

typedef struct UI_Box UI_Box;
struct UI_Box
{
 // rjf: hash links (persists across frames)
 UI_Box *hash_next;
 UI_Box *hash_prev;
 
 // rjf: tree link data (updated every frame)
 UI_Box *first;
 UI_Box *last;
 UI_Box *next;
 UI_Box *prev;
 UI_Box *parent;
 U64 child_count;
 
 // rjf: key info
 UI_Key key;
 
 // rjf: per-frame parameters
 UI_BoxFlags flags;
 String8 string;
 UI_Size pref_size[Axis2_COUNT];
 Axis2 child_layout_axis;
 OS_CursorKind hover_cursor;
 F32 opacity;
 UI_BoxTextExt             *ext_text;
 UI_BoxRectStyleExt        *ext_rect_style;
 UI_BoxBucketExt           *ext_bucket;
 UI_BoxCustomDrawExt       *ext_custom_draw;
 
 // rjf: post-size-determination data
 Vec2F32 calc_size;
 Vec2F32 calc_rel_pos;
 
 // rjf: post-layout data
 Rng2F32 rel_rect;
 Vec2F32 rel_corner_delta[Corner_COUNT];
 Rng2F32 rect;
 
 // rjf: persistent cross-frame state
 F32 hot_t;
 F32 active_t;
 F32 disabled_t;
 F32 focus_hot_t;
 F32 focus_active_t;
 U64 first_gen_touched;
 U64 last_gen_touched;
 Vec2F32 view_off;
 Vec2F32 target_view_off;
};


read_only UI_Box ui_g_nil_box =
{
 &ui_g_nil_box,
 &ui_g_nil_box,
 &ui_g_nil_box,
 &ui_g_nil_box,
 &ui_g_nil_box,
 &ui_g_nil_box,
 &ui_g_nil_box,
};

root_function B32 UI_BoxIsNil(UI_Box *box);
#define UI_BoxSetNil(b) ((b) = &ui_g_nil_box)
root_function UI_BoxRec UI_BoxRecurseDepthFirst(UI_Box *box, UI_Box *stopper, MemberOffset sib, MemberOffset child);
#define UI_BoxRecurseDepthFirstPre(box, stopper) UI_BoxRecurseDepthFirst((box), (stopper), MemberOff(UI_Box, next), MemberOff(UI_Box, first))
#define UI_BoxRecurseDepthFirstPost(box, stopper) UI_BoxRecurseDepthFirst((box), (stopper), MemberOff(UI_Box, prev), MemberOff(UI_Box, last))

root_function UI_BoxRec
UI_BoxRecurseDepthFirst(UI_Box *box, UI_Box *stopper, MemberOffset sib, MemberOffset child)
{
 UI_BoxRec rec = {0};
 rec.next = &ui_g_nil_box;
 if(!UI_BoxIsNil(MemberFromOff(box, UI_Box *, child)))
 {
  rec.next = MemberFromOff(box, UI_Box *, child);
  rec.push_count = 1;
 }
 else for(UI_Box *b = box; !UI_BoxIsNil(b) && b != stopper; b = b->parent)
 {
  if(!UI_BoxIsNil(MemberFromOff(b, UI_Box *, sib)))
  {
   rec.next = MemberFromOff(b, UI_Box *, sib);
   break;
  }
  rec.pop_count += 1;
 }
 return rec;
}
