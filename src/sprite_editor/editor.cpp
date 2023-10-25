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

    Imgui::window_rect_push(window_rect);

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
        const u64 sprite_index_in_selection = find(ctx.sprites_selected, i);
        const bool is_sprite_selected = sprite_index_in_selection != ctx.sprites_selected.size;

        {   // Sprite name
            char buffer[128];
            sprintf(buffer, "Sprite %d", i);

            const String text  = ref(buffer);
            const Vector2 size = Imgui::get_rendered_text_size(text, ctx.ui_font);

            if (!is_sprite_selected)
            {
                Rect rect = Rect {
                    top_left.x,
                    top_left.y,
                    top_left.x + window_width,
                    top_left.y + size.y + 2 * padding_y
                };

                if (Imgui::render_text_button(imgui_gen_id_with_secondary(i), rect, text, ctx.ui_font, Vector2 { padding_x, padding_y }, z))
                {
                    if (Input::get_key(Key::SHIFT))
                    {
                        if (is_sprite_selected)
                            remove(ctx.sprites_selected, sprite_index_in_selection);
                        else
                            append(ctx.sprites_selected, i);
                    }
                    else
                    {
                        clear(ctx.sprites_selected);
                        
                        if (!is_sprite_selected)
                            append(ctx.sprites_selected, i);
                    }
                }

                top_left.y += size.y + 3 * padding_y;
            }
            else
            {
                Imgui::render_text(text, ctx.ui_font, top_left, z);
                top_left.y += size.y + padding_y;

                {   // Delete Button
                    String btn_text       = ref("Delete");
                    Vector2 btn_text_size = Imgui::get_rendered_text_size(btn_text, ctx.ui_font);

                    Rect rect = Rect {
                        top_left.x,
                        top_left.y,
                        top_left.x + btn_text_size.x + 2 * padding_x,
                        top_left.y + btn_text_size.y + 2 * padding_y
                    };

                    if (Imgui::render_text_button(imgui_gen_id_with_secondary(i), rect, btn_text, ctx.ui_font, Vector2 { padding_x, padding_y }, z))
                        append(ctx.sprites_to_be_deleted, i);

                    top_left.y += btn_text_size.y + 3 * padding_y;
                }
            }

            z -= 0.001f;
        }

        top_left.y += padding_y;
    }

    Imgui::window_rect_pop();
}