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
    // ImGui::ShowDemoWindow();
    
    if (ImGui::BeginTabBar("EditTabBar", 0)) {
        if (ImGui::BeginTabItem("Add")) {
            ImGui::SeparatorText("Add New Entity");
            ImGui::Text("Click and select grid position");
            if (ImGui::Button("Create")) {
                addEntity(entity_manager, engine);
                entity_manager.update();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Edit")) {
            size_t e_id = engine->selectedEntityId();
            if (e_id) {
                char header[128];
                snprintf(header, sizeof(header), "Edit Entity: %lu", e_id);
                ImGui::SeparatorText(header);
                auto e = entity_manager.getEntity(e_id);
                if (e != nullptr) {
                    if (ImGui::TreeNode("Transform")) {
                        if (e->hasComponent<CTransform>()) {
                            auto& transform = e->getComponent<CTransform>();
                            ImGui::SetNextItemWidth(100);
                            ImGui::InputFloat("x", &transform.pos.x, 1.0f, 1.0f, "%.0f");
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(100);
                            ImGui::InputFloat("y", &transform.pos.y, 1.0f, 1.0f, "%.0f");
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Gravitation")) {
                        if (e->hasComponent<CGravity>()) {
                            auto& gravity = e->getComponent<CGravity>();
                            ImGui::SetNextItemWidth(100); 
                            ImGui::InputFloat("Gravity", &gravity.gravity, 0.25f, 0.25f, "%.2f");
                        } else if (ImGui::Button("Add gravity")) {
                            e->addComponent<CGravity>();
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Bounding box")) {
                        if (e->hasComponent<CBBox>()) {
                            auto& bbox = e->getComponent<CBBox>();
                            ImGui::SetNextItemWidth(100);
                            ImGui::InputFloat("Width", &bbox.size.x, 1.0f, .20f, "%.0f");
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(100);
                            ImGui::InputFloat("Height", &bbox.size.y, 1.0f, 1.0f, "%.0f");
                            ImGui::Checkbox("Block movement", &bbox.block_movement);
                            ImGui::SameLine();
                            ImGui::Checkbox("Block vision", &bbox.block_vision);
                            bbox.half_size.x = bbox.size.x/2;
                            bbox.half_size.y = bbox.size.y/2;
                        } else if (ImGui::Button("Add bounding box")) {
                            e->addComponent<CBBox>();
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Animation")) {
                        if (e->hasComponent<CAnimation>()) {
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
                            ImGui::ListBox("Animations", &cur_index, animations, IM_ARRAYSIZE(animations), 6);
                            if (prev_index != cur_index) {
                                e->addComponent<CAnimation>(engine->assets().getAnimation(animations[cur_index]), true);
                            }
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Health")) {
                        if (e->hasComponent<CHealth>()) {
                            auto& hp = e->getComponent<CHealth>();
                            ImGui::SetNextItemWidth(100);
                            ImGui::InputInt("Hp", &hp.current);
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(100);
                            ImGui::InputInt("Max hp", &hp.max);
                            if (hp.max < hp.current) {
                                hp.max = hp.current;
                            }
                            if (hp.current < 1) {
                                hp.current = 1; 
                            }
                            hp.percentage = static_cast<float>(hp.current)/static_cast<float>(hp.max);
                        } else if (ImGui::Button("Add health")) {
                            e->addComponent<CHealth>();
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Damage")) {
                        if (e->hasComponent<CDamage>()) {
                            auto& damage = e->getComponent<CDamage>();
                            ImGui::SetNextItemWidth(100);
                            ImGui::InputInt("Damage", &damage.damage);
                        } else if (ImGui::Button("Add damage")) {
                            e->addComponent<CDamage>();
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Invincibility")) {
                        if (e->hasComponent<CInvincibility>()) {
                            auto& invincibility = e->getComponent<CInvincibility>();
                            ImGui::SetNextItemWidth(100);
                            ImGui::InputInt("Time (frames)", &invincibility.i_frames, 60, 60);
                        } else if (ImGui::Button("Add invincibility")) {
                            e->addComponent<CInvincibility>();
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Type")) {
                        (void)m_types;
                        ImGui::TreePop();
                    }
            
                    if (ImGui::Button("Delete")) {
                        deleteEntity(e);
                        entity_manager.update();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Save")) {
                        // TODO: Update level file
                    }
                } else {
                    ImGui::Text("Entity was deleted.");
                }
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();

    ImGui::SFML::Render(window);
}

void Editor::processEvent(const sf::RenderWindow& window, const sf::Event& event) {
    ImGui::SFML::ProcessEvent(window, event);
}

std::shared_ptr<Entity> Editor::addEntity(EntityManager& entity_manager, GameEngine* engine) {
    auto tile = entity_manager.addEntity(Tag::TILE);
    tile->addComponent<CAnimation>(engine->assets().getAnimation("Brick"), true); // Note: use last added animation always?
    tile->addComponent<CTransform>(engine->selectedPos());
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
}

void Editor::deleteEntity(std::shared_ptr<Entity> e) {
    e->destroy();

    // Delete entity e data from file in m_level_path
}

bool Editor::windowActive() const {
    auto& io = ImGui::GetIO();
	return io.WantCaptureMouse || io.WantCaptureKeyboard;
}
