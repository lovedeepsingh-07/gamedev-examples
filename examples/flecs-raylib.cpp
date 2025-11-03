#include <cstdio>
#include <flecs.h>
#include <raylib.h>
#include <string>

// OnLoad - load data into your registry, keyboard/mouse inputs
// PostLoad - associate key presses with higher level actions
// PreUpdate - (after-loading/before-updating), like prepare/clean the frame
// OnUpdate - all the game-logic (default for adding systems)
// OnValidate - validate updates, like collision-detection
// PostUpdate - apply changes based on validations made in onValidate
// PreStore - (after-update/before-rendering)
// OnStore - begin rendering
// 	- Render_Game - rendering textures,3d models, and effects
// 	- Render_UI - render the UI elements
// 	- Finish Render - stop rendering

const constexpr int SCREEN_WIDTH = 1280;
const constexpr int SCREEN_HEIGHT = 720;
const constexpr int TARGET_FPS = 90;

namespace components {
    struct Position {
        float x;
        float y;
    };
    struct Rectangle {
        float width;
        float height;
        Color color;
    };
    struct Text {
        std::string text;
        int font_size;
        Color color;
    };
    namespace phases {
        struct Render_Game {};
        struct Render_UI {};
        struct Finish_Render {};
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "flecs-raylib");
    SetTargetFPS(TARGET_FPS);
    SetExitKey(0);

    flecs::world registry;
    registry.set_target_fps(TARGET_FPS);

    registry.component<components::Position>();
    registry.component<components::Rectangle>();
    registry.component<components::Text>();

    registry.entity<components::phases::Render_Game>().add(flecs::Phase).add(flecs::DependsOn, flecs::OnStore);
    registry.entity<components::phases::Render_UI>()
        .add(flecs::Phase)
        .add(flecs::DependsOn, registry.id<components::phases::Render_Game>());
    registry.entity<components::phases::Finish_Render>()
        .add(flecs::Phase)
        .add(flecs::DependsOn, registry.id<components::phases::Render_UI>());

    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Rectangle{ .width = 120, .height = 24, .color = RAYWHITE });
    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Text{
            .text = std::string("hello, world!"), .font_size = 24, .color = BLUE });

    registry.system().kind(flecs::OnStore).each([](flecs::entity curr_entity) {
        BeginDrawing();
        ClearBackground(BLACK);
    });
    registry.system<components::Rectangle, components::Position>()
        .kind<components::phases::Render_Game>()
        .each([](flecs::entity curr_entity, const components::Rectangle& rect,
                 const components::Position& pos) {
            DrawRectangle((int)pos.x, (int)pos.y, (int)rect.width, (int)rect.height, rect.color);
        });
    registry.system<components::Text, components::Position>()
        .kind<components::phases::Render_UI>()
        .each([](flecs::entity curr_entity, const components::Text& text,
                 const components::Position& pos) {
            DrawText(text.text.c_str(), (int)pos.x, (int)pos.y, text.font_size, text.color);
        });
    registry.system().kind<components::phases::Finish_Render>().each(
        [](flecs::entity curr_entity) { EndDrawing(); }
    );

    registry.app().enable_stats().enable_rest().run();

    CloseWindow();
    return 0;
}
