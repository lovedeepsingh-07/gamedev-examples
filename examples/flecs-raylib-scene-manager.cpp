#include "components.hpp"
#include "constants.hpp"
#include <cstdio>
#include <flecs.h>
#include <raylib.h>
#include <string>

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
    InitWindow(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, "flecs-raylib-scene-manager");
    SetTargetFPS(constants::TARGET_FPS);
    SetExitKey(0);

    flecs::world registry;
    registry.set_target_fps(constants::TARGET_FPS);
    registry.entity("scene_root").set_alias("scene_root").add<components::SceneRoot>();
    components::setup(registry);

    registry.observer<components::ActiveScene>()
        .event(flecs::OnAdd)
        .second<components::scenes::MainMenu>()
        .each(MainMenu_OnEnter);
    registry.observer<components::ActiveScene>()
        .event(flecs::OnAdd)
        .second<components::scenes::Game>()
        .each(Game_OnEnter);
    registry.observer<components::events::GameQuitEvent>().event(flecs::OnAdd).run([](flecs::iter& iter) {
        flecs::world registry = iter.world();
        // any logic that should run before quits should happen here, e.g unloading textures, etc
        registry.quit();
    });

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
        .kind<components::phases::OnUpdate>()
        .run([](flecs::iter& iter) {
            flecs::world registry = iter.world();
            if (IsKeyPressed(KEY_ONE)) {
                registry.add<components::ActiveScene, components::scenes::MainMenu>();
            }
            if (IsKeyPressed(KEY_TWO)) {
                registry.add<components::ActiveScene, components::scenes::Game>();
            }
            if (WindowShouldClose()) {
                registry.add<components::events::GameQuitEvent>();
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

    CloseWindow();
    return 0;
}
