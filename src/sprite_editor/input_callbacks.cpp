#include "input_callbacks.h"

#include "core/input.h"
#include "math/math.h"
#include "context.h"

void on_mouse_scroll(Application &app, s32 scroll)
{
    Context& ctx = *(Context*) app.data;
    
    if (ctx.input_state == EditorInputState::DRAG_IMAGE)
        return;

    Vector2 mouse_pos = Input::mouse_position();

    constexpr f32 scroll_zoom_sensitivity = 0.5f;
    f32 new_scale = ctx.image_scale + (0.15f * ctx.image_scale) * scroll_zoom_sensitivity * scroll;
    new_scale = clamp(new_scale, 0.5f, 50.0f);

    f32 ratio = new_scale / ctx.image_scale;

    ctx.image_top_left = (1.0f - ratio) * mouse_pos + ratio * ctx.image_top_left;
    ctx.image_scale = new_scale;
}