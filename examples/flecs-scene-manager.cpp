#include <cstdio>
#include <flecs.h>

// components
struct SceneRoot {};
struct ActiveScene {};
struct Common_Scene {};
struct MainMenu_Scene {};
struct Game_Scene {};
struct MainMenu_Pipeline {
    flecs::entity pipeline;
};
struct Game_Pipeline {
    flecs::entity pipeline;
};

// observer callbacks
void MainMenu_OnEnter(flecs::iter& iter, size_t, ActiveScene) {
    flecs::world registry = iter.world();
    printf("MainMenu::on_enter\n");

    registry.set_pipeline(registry.get<MainMenu_Pipeline>().pipeline);
}
void Game_OnEnter(flecs::iter& iter, size_t, ActiveScene) {
    flecs::world registry = iter.world();
    printf("Game::on_enter\n");

    registry.set_pipeline(registry.get<Game_Pipeline>().pipeline);
}

int main() {
    flecs::world registry;
    registry.set_target_fps(90);

    // register components
    registry.component<SceneRoot>();
    registry.component<ActiveScene>().add(flecs::Relationship).add(flecs::Exclusive);
    registry.component<MainMenu_Scene>().add(flecs::Target);
    registry.component<Game_Scene>().add(flecs::Target);
    registry.component<MainMenu_Pipeline>().add(flecs::Singleton);
    registry.component<Game_Pipeline>().add(flecs::Singleton);

    // add pipelines
    registry.set(MainMenu_Pipeline{ .pipeline = registry.pipeline()
                                                    .with(flecs::System)
                                                    .with<MainMenu_Scene>()
                                                    .oper(flecs::Or)
                                                    .with<Common_Scene>()
                                                    .build() });
    registry.set(Game_Pipeline{ .pipeline = registry.pipeline()
                                                .with(flecs::System)
                                                .with<Game_Scene>()
                                                .oper(flecs::Or)
                                                .with<Common_Scene>()
                                                .build() });

    // setup observers
    registry.observer<ActiveScene>()
        .event(flecs::OnAdd)
        .second<MainMenu_Scene>()
        .each(MainMenu_OnEnter);
    registry.observer<ActiveScene>().event(flecs::OnAdd).second<Game_Scene>().each(Game_OnEnter);

    // setup systems
    registry.system().kind<Common_Scene>().each([](flecs::entity curr_entity) {
        printf("common system running\n");
    });
    registry.system().kind<MainMenu_Scene>().each([](flecs::entity curr_entity) {
        printf("MainMenu system running\n");
    });
    registry.system().kind<Game_Scene>().each([](flecs::entity curr_entity) {
        printf("Game system running\n");
    });

    // testing
    registry.progress();
    printf("------progress-end------\n");
    registry.add<ActiveScene, MainMenu_Scene>();
    printf("------scene-change------\n");
    registry.progress();
    printf("------progress-end------\n");

    registry.add<ActiveScene, Game_Scene>();
    printf("------scene-change------\n");
    registry.progress();
    printf("------progress-end------\n");
    registry.progress();
    printf("------progress-end------\n");
    registry.progress();
    printf("------progress-end------\n");

    registry.add<ActiveScene, MainMenu_Scene>();
    printf("------scene-change------\n");

    registry.progress();
    printf("------progress-end------\n");

    registry.add<ActiveScene, MainMenu_Scene>();
    printf("------no-scene-change------\n");
    registry.progress();
    printf("------progress-end------\n");
    registry.progress();
    printf("------progress-end------\n");
    registry.progress();
    printf("------progress-end------\n");

    return 0;
}
