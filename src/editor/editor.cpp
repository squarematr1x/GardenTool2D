#include "editor.hpp"

#include "../ecs/entity-manager.hpp"
#include "../engine.hpp"

#include "../../vendor/imgui.h"
#include "../../vendor/imgui-SFML.h"

Editor::~Editor() {
    ImGui::SFML::Shutdown();
}

void Editor::init(sf::RenderWindow& window) {
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Cannot initialize editor\n";
    }
}

void Editor::update(sf::RenderWindow& window, EntityManager& entity_manager, GameEngine* engine) {
    ImGui::SFML::Update(window, m_dt.restart());

    ImGui::Begin("Editor");
    ImGui::SeparatorText("Add entity");

    ImGui::InputFloat("x", &m_entity_config.pos.x); // Only display this since its drag n dropped later
    ImGui::InputFloat("y", &m_entity_config.pos.y); // Only display this since its drag n dropped later
    ImGui::InputFloat("Bounding box width", &m_entity_config.bbox.x);
    ImGui::InputFloat("Bounding box height", &m_entity_config.bbox.y);
    ImGui::Checkbox("Block movement", &m_entity_config.block_movement);
    ImGui::Checkbox("Block vision", &m_entity_config.block_vision);
    ImGui::InputText("Animation name", m_entity_config.animation_name, sizeof(m_entity_config.animation_name)/sizeof(char));

    static int type = 0;
    const char* elems_names[11] = {
        "TILE",
        "DEC",
        "PLAYER",
        "ENEMY",
        "BULLET",
        "EXPLOSION",
        "SWORD",
        "HEART",
        "TELEPORT",
        "ELEVATOR",
	    "CHECKPOINT"
    };
    const char* elem_name = (type >= 0 && type < 11) ? elems_names[type] : "UNKNOWN";
    ImGui::SliderInt("Entity type", &type, 0, 11 - 1, elem_name);

    if (ImGui::Button("Create")) {
        addEntity(entity_manager, engine);
    }

    size_t e_id = engine->selectedEntityId();
    if (e_id > 0) {
        if (ImGui::Button("Delete")) {
            // std::cout << engine->currentLevelPath() << '\n';
            auto e = entity_manager.getEntity(e_id);
            if (e != nullptr) {
                deleteEntity(e);
            }
        }
    }

    ImGui::ShowDemoWindow();
    ImGui::End();

    ImGui::SFML::Render(window);
}

void Editor::processEvent(const sf::RenderWindow& window, const sf::Event& event) {
    ImGui::SFML::ProcessEvent(window, event);
}

std::shared_ptr<Entity> Editor::addEntity(EntityManager& entity_manager, GameEngine* engine) {
    auto tile = entity_manager.addEntity(Tag::TILE);
    tile->addComponent<CAnimation>(engine->assets().getAnimation("Brick"), true);
    tile->addComponent<CTransform>(Vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y));
    tile->addComponent<CDraggable>(); // TODO: Add draggable to other entities later
    return nullptr;
    // Store entity data (in m_entity_config) to file in m_level_path
    //  - Maybe: should also remove the previous entity in the same position, given that those have same type?
    // Spawn the entity to user's mouse position
}

void Editor::modifyEntity(std::shared_ptr<Entity> e, GameEngine* engine) {
    (void)e;
    (void)engine;
    // Store entity e data to file in m_level_path
    // 1.
}

void Editor::deleteEntity(std::shared_ptr<Entity> e) {
    e->destroy();
    // Delete entity e data from file in m_level_path
}
