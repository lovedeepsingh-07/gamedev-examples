#include "components.hpp"
#include "constants.hpp"
#include <cstdio>
#include <flecs.h>

// observer callbacks
void MainMenu_OnEnter(flecs::iter& iter, size_t, components::ActiveScene) {
    flecs::world registry = iter.world();
    printf("MainMenu::on_enter\n");

    registry.set_pipeline(registry.get<components::pipelines::MainMenu>().pipeline);
}
void Game_OnEnter(flecs::iter& iter, size_t, components::ActiveScene) {
    flecs::world registry = iter.world();
    printf("Game::on_enter\n");

    registry.set_pipeline(registry.get<components::pipelines::Game>().pipeline);
}

int main() {
    flecs::world registry;
    registry.set_target_fps(90);

    // register components
    registry.component<components::SceneRoot>();
    registry.component<components::ActiveScene>().add(flecs::Relationship).add(flecs::Exclusive);
    registry.component<components::scenes::MainMenu>().add(flecs::Target);
    registry.component<components::scenes::Game>().add(flecs::Target);
    registry.component<components::pipelines::MainMenu>().add(flecs::Singleton);
    registry.component<components::pipelines::Game>().add(flecs::Singleton);

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
                        .with<components::scenes::MainMenu>()
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

    // setup systems
    registry.system().kind<components::scenes::MainMenu>().run([](flecs::iter& iter) {
        printf("common system running\n");
    });
    registry.system().kind<components::scenes::MainMenu>().run([](flecs::iter& iter) {
        printf("MainMenu system running\n");
    });
    registry.system().kind<components::scenes::Game>().run([](flecs::iter& iter) {
        printf("Game system running\n");
    });

    // testing
    registry.progress();
    registry.add<components::ActiveScene, components::scenes::MainMenu>();
    registry.progress();
    registry.add<components::ActiveScene, components::scenes::Game>();
    registry.progress();
    registry.progress();
    registry.progress();
    registry.add<components::ActiveScene, components::scenes::MainMenu>();
    registry.progress();
    registry.add<components::ActiveScene, components::scenes::MainMenu>();
    registry.progress();
    registry.progress();
    registry.progress();

    return 0;
}
