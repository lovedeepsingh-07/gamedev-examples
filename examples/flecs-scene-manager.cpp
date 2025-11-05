#include "components.hpp"
#include "constants.hpp"
#include <cstdio>
#include <flecs.h>

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
    registry.entity("scene_root").set_alias("scene_root").add<components::SceneRoot>();
    components::setup(registry);

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

    registry.observer<components::ActiveScene>()
        .event(flecs::OnAdd)
        .second<components::scenes::MainMenu>()
        .each(MainMenu_OnEnter);
    registry.observer<components::ActiveScene>()
        .event(flecs::OnAdd)
        .second<components::scenes::Game>()
        .each(Game_OnEnter);

    registry.system()
        .run([](flecs::iter& iter) { printf("common system running\n"); })
        .add<components::scenes::Common>();
    registry.system()
        .run([](flecs::iter& iter) { printf("MainMenu system running\n"); })
        .add<components::scenes::MainMenu>();
    registry.system()
        .run([](flecs::iter& iter) { printf("Game system running\n"); })
        .add<components::scenes::Game>();

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
