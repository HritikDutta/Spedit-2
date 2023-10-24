#include "editor.h"

#include "application/application.h"
#include "core/input.h"
#include "context.h"

void render_sprite_list(const Application &app, Context &ctx, f32& z)
{
    SpriteSheet& sheet = ctx.sprite_sheet;
    
    constexpr f32 window_width = 200.0f;
    constexpr f32 padding_x = 10.0f;
    constexpr f32 padding_y = 5.0f;

    Rect window_rect = Rect {
        app.window.ref_width - window_width - 2 * padding_x,
        0.0f,
        (f32) app.window.ref_width,
        (f32) app.window.ref_height
    };

    Imgui::render_overlap_rect(imgui_gen_id(), window_rect, z, Vector4 { 0.0f, 0.0f, 0.0f, 0.25f });
    z -= 0.001f;

    Imgui::set_window_bounds(window_rect);

    Vector2 top_left = window_rect.top_left + Vector2 { padding_x, padding_y };
    
    {   // Label
        const String text  = ref("Sprites");
        const Vector2 size = Imgui::get_rendered_text_size(text, ctx.ui_font);

        Imgui::render_text(text, ctx.ui_font, top_left, z);
        top_left.y += size.y + padding_y;
        z -= 0.001f;
    }

    for (int i = 0; i < sheet.sprites.size; i++)
    {
        {   // Sprite name
            char buffer[128];
            sprintf(buffer, "Sprite %d", i);

            const String text  = ref(buffer);
            const Vector2 size = Imgui::get_rendered_text_size(text, ctx.ui_font);

            if (i != ctx.selected_sprite_index)
            {
                Rect rect = Rect {
                    top_left.x,
                    top_left.y,
                    top_left.x + window_width,
                    top_left.y + size.y + 2 * padding_y
                };

                if (Imgui::render_text_button(imgui_gen_id_with_secondary(i), rect, text, ctx.ui_font, Vector2 { padding_x, padding_y }, z))
                    ctx.selected_sprite_index = i;

                top_left.y += size.y + 3 * padding_y;
            }
            else
            {
                Imgui::render_text(text, ctx.ui_font, top_left, z);
                top_left.y += size.y + padding_y;
            }

            z -= 0.001f;
        }

        top_left.y += padding_y;
    }
}