#pragma once

#include "containers/string.h"
#include "engine/imgui.h"
#include "engine/rect.h"
#include "engine/sprite.h"
#include "math/vecs/vector2.h"

enum struct EditorInputState
{
    NONE,
    DRAG_IMAGE,
    CREATE_SPRITE,

    NUM_STATES
};

struct Context
{
    // Stuff that's constant
    Imgui::Font ui_font = {};

    // Can change based on image
    Imgui::Image background_image = {};
    String filename = {};

    SpriteSheet sprite_sheet = {};
    DynamicArray<Animation2D> animations = {};

    // State
    Vector2 image_top_left;
    f32 image_scale = 5.0f;

    EditorInputState input_state = EditorInputState::NONE;
    union
    {
        struct
        {
            Vector2 start_position;
            Vector2 start_image_top_left;
        } drag_image;

        struct
        {
            Vector2 start_position;
            Rect    sprite_rect;
        } create_sprite;
    };

    s32 selected_sprite_index = -1;
};

bool context_init(Context& ctx, const Application& app);
void context_free(Context& ctx);

// TODO: Handle errors for wrong initialization
void context_update_on_image_load(Context& ctx, const String file_path, const TextureSettings settings);