#pragma once

#include "containers/string.h"
#include "engine/imgui.h"
#include "math/vecs/vector2.h"
#include "engine/rect.h"

struct Context
{
    // Stuff that's constant
    Imgui::Font ui_font = {};

    // Can change based on image
    Imgui::Image background_image = {};
    Imgui::Image sprite_sheet = {};
    String filename = {};

    // State
    Vector2 image_top_left;
    f32 image_scale = 5.0f;

    bool is_dragging = false;
    Vector2 drag_start_position;
    Vector2 drag_start_image_top_left;

    bool is_holding_lmb = false;
    Vector2 lmb_start_position;
    Rect lmb_hold_rect;
};

bool context_init(Context& ctx, const Application& app);
void context_free(Context& ctx);

// TODO: Handle errors for wrong initialization
void context_update_on_image_load(Context& ctx, const String file_path, const TextureSettings settings);