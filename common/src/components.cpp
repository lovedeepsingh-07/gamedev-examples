#include "components.hpp"

void components::setup(flecs::world& registry) {
    registry.component<components::SceneRoot>();
    registry.component<components::ActiveScene>().add(flecs::Relationship).add(flecs::Exclusive);
    registry.component<components::scenes::MainMenu>().add(flecs::Target);
    registry.component<components::scenes::Game>().add(flecs::Target);
    registry.component<components::pipelines::MainMenu>().add(flecs::Singleton);
    registry.component<components::pipelines::Game>().add(flecs::Singleton);

    registry.component<components::phases::Phase>();
    registry.component<components::phases::OnUpdate>().add<components::phases::Phase>();
    registry.component<components::phases::OnRender_Start>()
        .add<components::phases::Phase>()
        .depends_on<components::phases::OnUpdate>();
    registry.component<components::phases::OnRender_Game>()
        .add<components::phases::Phase>()
        .depends_on<components::phases::OnRender_Start>();
    registry.component<components::phases::OnRender_UI>()
        .add<components::phases::Phase>()
        .depends_on<components::phases::OnRender_Game>();
    registry.entity<components::phases::OnRender_Debug>()
        .add<components::phases::Phase>()
        .depends_on<components::phases::OnRender_UI>();
    registry.entity<components::phases::OnRender_Finish>()
        .add<components::phases::Phase>()
        .depends_on<components::phases::OnRender_Debug>();

    registry.component<components::global_options::GameFonts>().add(flecs::Singleton);

    registry.component<components::events::GameQuitEvent>().add(flecs::Singleton);

    registry.component<components::Position>();
    registry.component<components::Rectangle>();
    registry.component<components::Text>();
}
