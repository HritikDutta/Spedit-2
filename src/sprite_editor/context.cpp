#include "context.h"

#include "application/application.h"
#include "containers/string.h"
#include "containers/darray.h"
#include "containers/algorithms.h"
#include "core/logger.h"
#include "engine/imgui.h"
#include "fileio/fileio.h"
#include "graphics/texture.h"
#include "platform/platform.h"
#include "serialization/slz.h"
#include "serialization/json.h"

static char filename_buffer[256] = {};

bool context_init(Context& ctx, const Application& app)
{
    {   // Load UI Font
        String content = file_load_string(ref("assets/fonts/assistant-medium.font.json"));

        Slz::Document document = {};
        if (!Json::parse_string(content, document))
        {
            print_error("Error parsing font json!");
            return false;
        }

        ctx.ui_font = Imgui::font_load_from_document(document, ref("assets/fonts/assistant-medium.font.png"));

        free(document);
        free(content);
    }

    {   // Create temp background image
        constexpr s32 width  = 128;
        constexpr s32 height = 128;

        u8* pixels = (u8*) platform_allocate(width * height * 4 * sizeof(u8));
        if (!pixels)
        {
            print_error("Couldn't allocate background image!");
            return false;
        }

        const u8 colors[][4] = {
            { 100, 100, 100, 200 },
            {  50,  50,  50, 200 },
        };

        for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            const u32 idx   = (y * width + x) * 4;
            const u8* color = colors[(x + y) % 2];

            pixels[idx + 0] = color[0];
            pixels[idx + 1] = color[1];
            pixels[idx + 2] = color[2];
            pixels[idx + 3] = color[3];
        }

        ctx.background_image = texture_load_pixels(ref("background"), pixels, width, height, 4, TextureSettings::default());
        ctx.sprite_sheet.atlas = texture_load_pixels(ref("sprite sheet"), nullptr, 0, 0, 4, TextureSettings::default());

        platform_free(pixels);
    }

    {   // Set sheet rect to middle of window
        ctx.image_top_left.x = 0.5f * (app.window.ref_width - 128 * ctx.image_scale);
        ctx.image_top_left.y = 0.5f * (app.window.ref_height - 128 * ctx.image_scale);
    }

    {   // File Name
        ctx.filename = ref(filename_buffer);
        string_copy_into(ctx.filename, ref("Empty"));
    }

    {   // Initialize structs for spritesheet and animation
        ctx.sprite_sheet.sprites = make<DynamicArray<SpriteSheet::SpriteData>>();
        ctx.animations = make<DynamicArray<Animation2D>>();
    }

    {   // Other state things
        ctx.sprites_selected = make<DynamicArray<s32>>();
        ctx.sprites_to_be_deleted = make<DynamicArray<s32>>();
    }

    return true;
}

void context_free(Context &ctx)
{
    free(ctx.background_image);
    free(ctx.sprite_sheet);
    free(ctx.ui_font);
}

void context_update_on_image_load(Context& ctx, const String filepath, const TextureSettings settings)
{
    texture_set_pixels_from_image(ctx.sprite_sheet.atlas, filepath, TextureSettings::default());

    {   // Save file name separately
        s64 last_slash_idx = filepath.size - 1;
        while (last_slash_idx >= 0 && filepath[last_slash_idx] != '/' && filepath[last_slash_idx] != '\\')
            last_slash_idx--;
        
        // Offset if the current character is a slash (the check is needed if the character isn't a slash)
        const s64 offset = (filepath[last_slash_idx] == '/' || filepath[last_slash_idx] == '\\');
        const s64 start_idx = last_slash_idx + offset;
        string_copy_into(ctx.filename, get_substring(filepath, start_idx));
    }
    
    const s32 width  = texture_get_width(ctx.sprite_sheet.atlas);
    const s32 height = texture_get_height(ctx.sprite_sheet.atlas);

    {   // Resize background
        u8* pixels = (u8*) platform_allocate(width * height * 4 * sizeof(u8));
        if (!pixels)
        {
            print_error("Couldn't allocate background image!");
            return;
        }

        const u8 colors[][4] = {
            { 100, 100, 100, 200 },
            {  50,  50,  50, 200 },
        };

        for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            const u32 idx   = (y * width + x) * 4;
            const u8* color = colors[(x + y) % 2];

            pixels[idx + 0] = color[0];
            pixels[idx + 1] = color[1];
            pixels[idx + 2] = color[2];
            pixels[idx + 3] = color[3];
        }

        texture_set_pixels(ctx.background_image, pixels, width, height, 4, TextureSettings::default());
        platform_free(pixels);
    }
    
    {   // Set sheet rect to middle of window
        const Application& app = application_get_active();
        ctx.image_top_left.x = 0.5f * (app.window.ref_width  - width * ctx.image_scale);
        ctx.image_top_left.y = 0.5f * (app.window.ref_height - height * ctx.image_scale);
    }

    {   // Reset sprite sheet and animation data
        clear(ctx.sprite_sheet.sprites);

        for (u64 i = 0; i < ctx.animations.size; i++)
            free(ctx.animations[i]);

        clear(ctx.animations);
    }

    {   // Reset any control states
        ctx.input_state = EditorInputState::NONE;
    }
}

void context_delete_sprites(Context &ctx)
{
    // Sort the list first
    sort(ctx.sprites_to_be_deleted);

    for (int i = ctx.sprites_to_be_deleted.size - 1; i >= 0; i--)
        remove(ctx.sprite_sheet.sprites, ctx.sprites_to_be_deleted[i]);

    clear(ctx.sprites_to_be_deleted);
}