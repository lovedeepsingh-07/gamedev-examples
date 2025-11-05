#include "components.hpp"
#include "constants.hpp"
#include <clay_raylib.hpp>
#include <cstdio>
#include <flecs.h>
#include <raylib.h>
#include <string>

Clay_Color raylib_color_to_clay_color(Color rayColor) {
    Clay_Color clayColor;
    clayColor.r = rayColor.r;
    clayColor.g = rayColor.g;
    clayColor.b = rayColor.b;
    clayColor.a = rayColor.a;
    return clayColor;
}

void clay_error_handler(Clay_ErrorData errorData) {
    printf("%s\n", errorData.errorText.chars);
}

void MainMenu_UI(flecs::world& registry) {
    CLAY({ .id = CLAY_ID("main_menu_MainContainer"),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                       .padding = CLAY_PADDING_ALL(16) } }) {
        CLAY_TEXT(
            CLAY_STRING("Main Menu"),
            CLAY_TEXT_CONFIG({
                .textColor = raylib_color_to_clay_color(WHITE),
                .fontId = 0,
                .fontSize = 25,
            })
        );
    }
}
void Game_UI(flecs::world& registry) {
    CLAY({ .id = CLAY_ID("game_MainContainer"),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                       .padding = CLAY_PADDING_ALL(16) } }) {
        CLAY_TEXT(
            CLAY_STRING("Game"),
            CLAY_TEXT_CONFIG({
                .textColor = raylib_color_to_clay_color(WHITE),
                .fontId = 0,
                .fontSize = 25,
            })
        );
    }
}

void MainMenu_OnEnter(flecs::iter& iter, size_t, components::ActiveScene) {
    flecs::world registry = iter.world();
    printf("MainMenu::on_enter\n");

    registry.defer_begin();
    registry.delete_with(flecs::ChildOf, registry.lookup("scene_root"));
    registry.defer_end();

    flecs::entity scene_root = registry.lookup("scene_root");

    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Rectangle{ .width = 120, .height = 24, .color = PURPLE })
        .child_of(scene_root);
    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Text{ .text = std::string("main_menu"), .font_size = 24, .color = RAYWHITE })
        .child_of(scene_root);

    registry.set_pipeline(registry.get<components::pipelines::MainMenu>().pipeline);
}
void Game_OnEnter(flecs::iter& iter, size_t, components::ActiveScene) {
    flecs::world registry = iter.world();
    printf("Game::on_enter\n");

    registry.defer_begin();
    registry.delete_with(flecs::ChildOf, registry.lookup("scene_root"));
    registry.defer_end();

    flecs::entity scene_root = registry.lookup("scene_root");

    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Rectangle{ .width = 120, .height = 24, .color = PURPLE })
        .child_of(scene_root);
    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Text{ .text = std::string("game"), .font_size = 24, .color = RAYWHITE })
        .child_of(scene_root);

    registry.set_pipeline(registry.get<components::pipelines::Game>().pipeline);
}

int main() {
    Clay_Raylib_Initialize(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, "flecs-raylib-scene-manager-clay", FLAG_MSAA_4X_HINT);
    SetTargetFPS(constants::TARGET_FPS);
    SetExitKey(0);

    flecs::world registry;
    registry.set_target_fps(constants::TARGET_FPS);
    registry.entity("scene_root").set_alias("scene_root").add<components::SceneRoot>();
    components::setup(registry);

    uint64_t clay_required_memory = Clay_MinMemorySize();
    auto clay_memory = Clay_Arena{
        .capacity = static_cast<size_t>(clay_required_memory),
        .memory = (char*)malloc(clay_required_memory),
    };
    Clay_Initialize(
        clay_memory,
        Clay_Dimensions{
            .width = (float)constants::SCREEN_WIDTH,
            .height = (float)constants::SCREEN_HEIGHT,
        },
        (Clay_ErrorHandler){ clay_error_handler }
    );
    std::array<Font, 1> font_list{
        LoadFontEx("assets/JetBrainsMonoNLNerdFontComplete-Regular.ttf", 48, NULL, 400)
    };
    SetTextureFilter(font_list[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, font_list.data());
    Clay_SetDebugModeEnabled(true);
    registry.set<components::global_options::GameFonts>({ font_list });

    registry.observer<components::ActiveScene>()
        .event(flecs::OnAdd)
        .second<components::scenes::MainMenu>()
        .each(MainMenu_OnEnter);
    registry.observer<components::ActiveScene>()
        .event(flecs::OnAdd)
        .second<components::scenes::Game>()
        .each(Game_OnEnter);

    registry.set(components::pipelines::MainMenu{
        .pipeline = registry.pipeline()
                        .with(flecs::System)
                        .with<components::phases::Phase>()
                        .cascade(flecs::DependsOn)
                        .with<components::scenes::MainMenu>()
                        .oper(flecs::Or)
                        .with<components::scenes::Common>()
                        .build() });
    registry.set(components::pipelines::Game{
        .pipeline = registry.pipeline()
                        .with(flecs::System)
                        .with<components::phases::Phase>()
                        .cascade(flecs::DependsOn)
                        .with<components::scenes::Game>()
                        .oper(flecs::Or)
                        .with<components::scenes::Common>()
                        .build() });

    // NOTE: the system definition order is jumbled in order to specify the working of custom phases
    registry.system<components::Text, components::Position>()
        .kind<components::phases::OnRender_UI>()
        .each([](flecs::entity curr_entity, const components::Text& text,
                 const components::Position& pos) {
            DrawText(text.text.c_str(), (int)pos.x, (int)pos.y, text.font_size, text.color);
        })
        .add<components::scenes::Common>();
    registry.system()
        .kind<components::phases::OnRender_UI>()
        .run([](flecs::iter& iter) {
            flecs::world registry = iter.world();
            auto game_fonts = registry.get<components::global_options::GameFonts>();
            Vector2 mouse_pos = GetMousePosition();
            Vector2 scroll_delta = GetMouseWheelMoveV();
            float delta_time = registry.delta_time();

            Clay_SetLayoutDimensions((Clay_Dimensions
            ){ .width = (float)GetScreenWidth(), .height = (float)GetScreenHeight() });
            Clay_SetPointerState(Clay_Vector2{ mouse_pos.x, mouse_pos.y }, IsMouseButtonDown(0));
            Clay_UpdateScrollContainers(
                true, (Clay_Vector2){ scroll_delta.x, scroll_delta.y }, delta_time
            );
            Clay_BeginLayout();
            if (registry.has<components::ActiveScene, components::scenes::MainMenu>()) {
                MainMenu_UI(registry);
            }
            if (registry.has<components::ActiveScene, components::scenes::Game>()) {
                Game_UI(registry);
            }
            Clay_RenderCommandArray render_commands = Clay_EndLayout();
            Clay_Raylib_Render(render_commands, game_fonts.font_list.data());
        })
        .add<components::scenes::Common>();
    registry.system()
        .kind<components::phases::OnUpdate>()
        .run([](flecs::iter& iter) {
            flecs::world registry = iter.world();
            if (IsKeyPressed(KEY_ONE)) {
                registry.add<components::ActiveScene, components::scenes::MainMenu>();
            }
            if (IsKeyPressed(KEY_TWO)) {
                registry.add<components::ActiveScene, components::scenes::Game>();
            }
        })
        .add<components::scenes::Common>();
    registry.system<components::Rectangle, components::Position>()
        .kind<components::phases::OnRender_Game>()
        .each([](flecs::entity curr_entity, const components::Rectangle& rect,
                 const components::Position& pos) {
            DrawRectangle((int)pos.x, (int)pos.y, (int)rect.width, (int)rect.height, BLUE);
        })
        .add<components::scenes::MainMenu>();
    registry.system<components::Rectangle, components::Position>()
        .kind<components::phases::OnRender_Game>()
        .each([](flecs::entity curr_entity, const components::Rectangle& rect,
                 const components::Position& pos) {
            DrawRectangle((int)pos.x, (int)pos.y, (int)rect.width, (int)rect.height, RED);
        })
        .add<components::scenes::Game>();
    registry.system()
        .kind<components::phases::OnRender_Finish>()
        .run([](flecs::iter& iter) { EndDrawing(); })
        .add<components::scenes::Common>();
    registry.system()
        .kind<components::phases::OnRender_Start>()
        .run([](flecs::iter& iter) {
            BeginDrawing();
            ClearBackground(BLACK);
        })
        .add<components::scenes::Common>();

    registry.add<components::ActiveScene, components::scenes::MainMenu>();
    registry.app().enable_stats().enable_rest().run();

    Clay_Raylib_Close();
    return 0;
}
