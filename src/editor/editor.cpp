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
    size_t id = engine->selectedEntityId();
    if (!id) {
        return;
    }

    ImGui::SFML::Update(window, m_dt.restart());

    ImGui::Begin("Editor");

    size_t e_id = engine->selectedEntityId();
    if (e_id) {
        ImGui::SeparatorText("Edit Entity");
        auto e = entity_manager.getEntity(e_id);
        if (e != nullptr) {
            if (ImGui::TreeNode("Transform")) {
                if (e->hasComponent<CTransform>()) {
                    auto& transform = e->getComponent<CTransform>();
                    ImGui::InputFloat("x", &transform.pos.x, 1.0f, 1.0f, "%.0f");
                    ImGui::InputFloat("y", &transform.pos.x, 1.0f, 1.0f, "%.0f");
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Bounding box")) {
                if (e->hasComponent<CBBox>()) {
                    auto& bbox = e->getComponent<CBBox>();
                    ImGui::InputFloat("Bounding box width", &bbox.size.x, 1.0f, 1.0f, "%.0f");
                    ImGui::InputFloat("Bounding box height", &bbox.size.y, 1.0f, 1.0f, "%.0f");
                    ImGui::Checkbox("Block movement", &bbox.block_movement);
                    ImGui::Checkbox("Block vision", &bbox.block_vision);
                    bbox.half_size.x = bbox.size.x/2;
                    bbox.half_size.y = bbox.size.y/2;
                } else {
                    if (ImGui::Button("Add bounding box")) {
                        e->addComponent<CBBox>();
                    }
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Animation")) {
                if (e->hasComponent<CAnimation>()) {
                    (void)m_types;

                    auto anims = engine->assets().getAnimations();
                    const char* animations[anims.size()];
                    const std::string anim_name = e->getComponent<CAnimation>().animation.getName();
                    static int cur_index = 1;
                    int i = 0;

                    for (const auto& pair : anims) {
                        animations[i] = pair.first.c_str();
                        if (pair.first == anim_name) {
                            cur_index = i;
                        }
                        i++;
                    }

                    int prev_index = cur_index;
                    ImGui::ListBox("Animation", &cur_index, animations, IM_ARRAYSIZE(animations), 6);
                    if (prev_index != cur_index) {
                        e->addComponent<CAnimation>(engine->assets().getAnimation(animations[cur_index]), true);
                    }
                }
                ImGui::TreePop();
            }
    
            if (ImGui::Button("Delete")) {
                deleteEntity(e);
            }

            if (ImGui::Button("Save")) {
                // Only update file when save pressed?
            }
        }
    } else {
        // ImGui::InputText("Animation name", m_entity_config.animation_name, sizeof(m_entity_config.animation_name)/sizeof(char));

        // if (ImGui::Button("Create")) { // NOTE: Only spawn create dialog when right clicking an entity
        //     addEntity(entity_manager, engine);
        // }
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

bool Editor::windowActive() const {
    auto& io = ImGui::GetIO();
	return io.WantCaptureMouse || io.WantCaptureKeyboard;
}
