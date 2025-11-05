#include "components.hpp"
#include "constants.hpp"
#include <cstdio>
#include <flecs.h>
#include <raylib.h>
#include <string>

int main() {
    InitWindow(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, "flecs-raylib");
    SetTargetFPS(constants::TARGET_FPS);
    SetExitKey(0);

    flecs::world registry;
    registry.set_target_fps(constants::TARGET_FPS);
    components::setup(registry);

    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Rectangle{ .width = 120, .height = 24, .color = RAYWHITE });
    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Text{
            .text = std::string("hello, world!"), .font_size = 24, .color = BLUE });

    registry.set_pipeline(registry.pipeline()
                              .with(flecs::System)
                              .with<components::phases::Phase>()
                              .cascade(flecs::DependsOn)
                              .build());

    // NOTE: the system definition order is jumbled in order to specify the working of custom phases
    registry.system().kind<components::phases::OnRender_Finish>().run(
        [](flecs::iter& iter) { EndDrawing(); }
    );
    registry.system<components::Rectangle, components::Position>()
        .kind<components::phases::OnRender_Game>()
        .each([](flecs::entity curr_entity, const components::Rectangle& rect,
                 const components::Position& pos) {
            DrawRectangle((int)pos.x, (int)pos.y, (int)rect.width, (int)rect.height, rect.color);
        });
    registry.system().kind<components::phases::OnRender_Start>().run([](flecs::iter& iter) {
        BeginDrawing();
        ClearBackground(BLACK);
    });
    registry.system<components::Text, components::Position>()
        .kind<components::phases::OnRender_UI>()
        .each([](flecs::entity curr_entity, const components::Text& text,
                 const components::Position& pos) {
            DrawText(text.text.c_str(), (int)pos.x, (int)pos.y, text.font_size, text.color);
        });

    registry.app().enable_stats().enable_rest().run();

    CloseWindow();
    return 0;
}
