#include <cstdio>
#include <flecs.h>
#include <raylib.h>
#include <string>

const constexpr int SCREEN_WIDTH = 1280;
const constexpr int SCREEN_HEIGHT = 720;
const constexpr int TARGET_FPS = 90;

namespace components {
    struct SceneRoot {};
    struct ActiveScene {};
    namespace scenes {
        struct Common {};
        struct MainMenu {};
        struct Game {};
    };
    namespace pipelines {
        struct MainMenu {
            flecs::entity pipeline;
        };
        struct Game {
            flecs::entity pipeline;
        };
    };
    namespace phases {
        struct Render_Game {};
        struct Render_UI {};
        struct Finish_Render {};
    }
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
}

void MainMenu_OnEnter(flecs::iter& iter, size_t, components::ActiveScene) {
    flecs::world registry = iter.world();

    registry.defer_begin();
    registry.delete_with(flecs::ChildOf, registry.lookup("scene_root"));
    registry.defer_end();

    printf("MainMenu::on_enter\n");

    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Rectangle{ .width = 120, .height = 24, .color = RAYWHITE });
    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Text{ .text = std::string("main menu"), .font_size = 24, .color = BLUE });

    registry.set_pipeline(registry.get<components::pipelines::MainMenu>().pipeline);
}
void Game_OnEnter(flecs::iter& iter, size_t, components::ActiveScene) {
    flecs::world registry = iter.world();

    registry.defer_begin();
    registry.delete_with(flecs::ChildOf, registry.lookup("scene_root"));
    registry.defer_end();

    printf("Game::on_enter\n");

    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Rectangle{ .width = 120, .height = 24, .color = RAYWHITE });
    registry.entity()
        .set(components::Position{ .x = 100, .y = 100 })
        .set(components::Text{ .text = std::string("game"), .font_size = 24, .color = BLUE });

    registry.set_pipeline(registry.get<components::pipelines::Game>().pipeline);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "flecs-raylib");
    SetTargetFPS(TARGET_FPS);
    SetExitKey(0);

    flecs::world registry;
    registry.set_target_fps(TARGET_FPS);
    registry.entity("scene_root").set_alias("scene_root").add<components::SceneRoot>();

    registry.component<components::SceneRoot>();
    registry.component<components::ActiveScene>().add(flecs::Relationship).add(flecs::Exclusive);
    registry.component<components::scenes::MainMenu>().add(flecs::Target);
    registry.component<components::scenes::Game>().add(flecs::Target);
    registry.component<components::pipelines::MainMenu>().add(flecs::Singleton);
    registry.component<components::pipelines::Game>().add(flecs::Singleton);
    registry.component<components::Position>();
    registry.component<components::Rectangle>();
    registry.component<components::Text>();

    // add pipelines
    registry.set(components::pipelines::MainMenu{
        .pipeline = registry.pipeline()
                        .with(flecs::System)
                        .with<components::scenes::MainMenu>()
                        .oper(flecs::Or)
                        .with<components::scenes::Common>()
                        .build() });
    registry.set(components::pipelines::Game{
        .pipeline = registry.pipeline()
                        .with(flecs::System)
                        .with<components::scenes::Game>()
                        .oper(flecs::Or)
                        .with<components::scenes::Common>()
                        .build() });

    // setup observers
    registry.observer<components::ActiveScene>()
        .event(flecs::OnAdd)
        .second<components::scenes::MainMenu>()
        .each(MainMenu_OnEnter);
    registry.observer<components::ActiveScene>()
        .event(flecs::OnAdd)
        .second<components::scenes::Game>()
        .each(Game_OnEnter);

    // setup phases
    registry.entity<components::phases::Render_Game>().add(flecs::Phase).add(flecs::DependsOn, flecs::OnStore);
    registry.entity<components::phases::Render_UI>()
        .add(flecs::Phase)
        .add(flecs::DependsOn, registry.id<components::phases::Render_Game>());
    registry.entity<components::phases::Finish_Render>()
        .add(flecs::Phase)
        .add(flecs::DependsOn, registry.id<components::phases::Render_UI>());

    // set to Main Menu
    registry.add<components::ActiveScene, components::scenes::MainMenu>();

    // setup systems
    registry.system().kind<components::scenes::Common>().kind(flecs::OnUpdate).each([](flecs::entity curr_entity) {
        flecs::world registry = curr_entity.world();
        if (IsKeyPressed(KEY_ONE)) {
            printf("one pressed\n");
            // registry.add<components::ActiveScene, components::scenes::MainMenu>();
        }
        if (IsKeyPressed(KEY_TWO)) {
            printf("two pressed\n");
            // registry.add<components::ActiveScene, components::scenes::Game>();
        }
    });
    registry.system().kind<components::scenes::Common>().kind(flecs::OnStore).each([](flecs::entity curr_entity) {
        BeginDrawing();
        ClearBackground(BLACK);
    });
    registry.system<components::Rectangle, components::Position>()
        .kind<components::scenes::Common>()
        .kind<components::phases::Render_Game>()
        .each([](flecs::entity curr_entity, const components::Rectangle& rect,
                 const components::Position& pos) {
            DrawRectangle((int)pos.x, (int)pos.y, (int)rect.width, (int)rect.height, rect.color);
        });
    registry.system<components::Text, components::Position>()
        .kind<components::scenes::Common>()
        .kind<components::phases::Render_UI>()
        .each([](flecs::entity curr_entity, const components::Text& text,
                 const components::Position& pos) {
            DrawText(text.text.c_str(), (int)pos.x, (int)pos.y, text.font_size, text.color);
        });
    registry.system()
        .kind<components::scenes::Common>()
        .kind<components::phases::Finish_Render>()
        .each([](flecs::entity curr_entity) { EndDrawing(); });

    registry.app().enable_stats().enable_rest().run();

    CloseWindow();
    return 0;
}
