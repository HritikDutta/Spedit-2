#include "application/application.h"
#include "platform/platform.h"
#include "sprite_editor/context.h"
#include "core/input.h"
#include "engine/rect.h"
#include "sprite_editor/input_callbacks.h"

bool on_init(Application& app)
{
    Context& ctx = *(Context*) app.data;

    context_init(ctx, app);

    Input::register_mouse_scroll_event_callback(on_mouse_scroll);

    return true;
}

void on_update(Application& app)
{
    Context& ctx = *(Context*) app.data;

    if (Input::get_key(Key::ESCAPE))
    {
        app.is_running = false;
        return;
    }

    {   // Move image
        if (Input::get_mouse_button(MouseButton::MIDDLE))
        {
            if (!ctx.is_dragging)
            {
                ctx.drag_start_image_top_left = ctx.image_top_left;
                ctx.drag_start_position = Input::mouse_position();
                ctx.is_dragging = true;
            }

            Vector2 diff = Input::mouse_position() - ctx.drag_start_position;
            ctx.image_top_left = ctx.drag_start_image_top_left + diff;
        }
        else if (ctx.is_dragging)
            ctx.is_dragging = false;
    }

    {   // Create Sprites
        if (Input::get_mouse_button(MouseButton::LEFT))
        {
            if (!ctx.is_holding_lmb)
            {
                ctx.lmb_start_position = Input::mouse_position() - ctx.image_top_left;
                ctx.is_holding_lmb = true;
            }
            
            const Vector2 mouse_position = Input::mouse_position() - ctx.image_top_left;
            ctx.lmb_hold_rect.left   = min(mouse_position.x, ctx.lmb_start_position.x) / ctx.image_scale;
            ctx.lmb_hold_rect.right  = max(mouse_position.x, ctx.lmb_start_position.x) / ctx.image_scale;
            ctx.lmb_hold_rect.top    = min(mouse_position.y, ctx.lmb_start_position.y) / ctx.image_scale;
            ctx.lmb_hold_rect.bottom = max(mouse_position.y, ctx.lmb_start_position.y) / ctx.image_scale;

            const f32 width  = texture_get_width(ctx.background_image);
            const f32 height = texture_get_height(ctx.background_image);

            ctx.lmb_hold_rect.left    = roundf(clamp(ctx.lmb_hold_rect.left,   0.0f, width) - 0.35f);
            ctx.lmb_hold_rect.top     = roundf(clamp(ctx.lmb_hold_rect.top,    0.0f, height) - 0.35f);
            ctx.lmb_hold_rect.right   = roundf(clamp(ctx.lmb_hold_rect.right,  0.0f, width) - 0.35f);
            ctx.lmb_hold_rect.bottom  = roundf(clamp(ctx.lmb_hold_rect.bottom, 0.0f, height) - 0.35f);
        }
        else if (ctx.is_holding_lmb)
            ctx.is_holding_lmb = false;
    }
}

void on_render(Application& app)
{
    Context& ctx = *(Context*) app.data;

    Imgui::begin();
    f32 z = 0.1f;
    
    Imgui::set_scale(ctx.image_scale, ctx.image_scale);
    Imgui::set_offset(ctx.image_top_left.x, ctx.image_top_left.y);

    {   // Show background texture
        Imgui::render_image(ctx.background_image, Vector2 {}, z);
        z -= 0.001f;
    }

    {   // Show sprite sheet
        if (texture_is_valid(ctx.sprite_sheet))
        {
            Imgui::render_image(ctx.sprite_sheet, Vector2 {}, z);
            z -= 0.001f;
        }
    }

    {   // Show LMB hold rect for new sprites
        if (ctx.is_holding_lmb)
        {
            Imgui::render_rect(ctx.lmb_hold_rect, z, Vector4 { 1.0f, 0.25f, 0.25f, 0.25f });
            z -= 0.001f;
        }
    }

    Imgui::set_scale(1, 1);
    Imgui::set_offset(0, 0);

    {   // File Info
        constexpr f32 padding_y = 5.0f;

        Vector2 top_left = Vector2 { 10.0f, 10.0f };

        {   // File Name
            char buffer[128];
            sprintf(buffer, "File: %s", ctx.filename.data);

            String text = ref(buffer);        
            Imgui::render_text(text, ctx.ui_font, top_left, z);
            z -= 0.001f;

            top_left.y += ctx.ui_font.size + padding_y;
        }

        {   // File Size
            char buffer[128];
            sprintf(buffer, "Size: %dx%d", texture_get_width(ctx.background_image), texture_get_height(ctx.background_image));

            String text = ref(buffer);
            Imgui::render_text(text, ctx.ui_font, top_left, z);
            z -= 0.001f;

            top_left.y += ctx.ui_font.size + padding_y;
        }

        {   // Open File Dialog
            const String text = ref("Open");
            const Vector2 size = Imgui::get_rendered_text_size(text, ctx.ui_font);
            const Vector2 padding = Vector2 { 10.0f, 5.0f };

            Rect rect;
            rect.top_left = top_left;
            rect.bottom_right = top_left + size + 2 * padding;

            if (Imgui::render_text_button(imgui_gen_id(), rect, text, ctx.ui_font, padding, z))
            {
                constexpr char filter[] = "Images (*.PNG, *.JPG, *.JPEG)\0*.PNG;*.JPG;*.JPEG\0PNG (*.PNG)\0*.PNG\0JPEG (*.JPG, *.JPEG)\0*.JPG;*.JPEG\0\0";
                char filename[512] = "";

                if (platform_dialogue_open_file(filter, filename, 512))
                    context_update_on_image_load(ctx, ref(filename), TextureSettings::default());
            }
            z -= 0.001f;

            top_left.y = rect.bottom + padding_y;
        }
    }

    Imgui::end();
}

void on_shutdown(Application& app)
{
    Context& ctx = *(Context*) app.data;
}

void on_window_resize(Application& app)
{

}

void create_app(Application& app)
{
    {   // Window setup
        app.window.x = 100;
        app.window.y = 100;
        app.window.width  = 1280;
        app.window.height = 720;
        app.window.name = ref("Spedit 2");
        app.window.style = WindowStyle::WINDOWED;
        app.window.icon_path = ref("assets/art/app_icon.png");
    }

    {   // Callbacks
        app.on_init     = on_init;
        app.on_update   = on_update;
        app.on_render   = on_render;
        app.on_shutdown = on_shutdown;
        app.on_window_resize = on_window_resize;
    }

    app.clear_color = Vector4(0.15f, 0.15f, 0.15f, 1.0f);

    app.data = platform_allocate(sizeof(Context));
    *(Context*) app.data = Context();
}