#include "editor.hpp"

#include <sstream>
 
#include "../../vendor/imgui.h"
#include "../../vendor/imgui-SFML.h"

#include "../ecs/entity-manager.hpp"
#include "../engine.hpp"
#include "../player-config.hpp"
#include "../util/file.hpp"

Editor::~Editor() {
    ImGui::SFML::Shutdown();
}

void Editor::init(sf::RenderWindow& window) {
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Cannot initialize editor\n";
    }
}

// NOTE: this function is rather messy, but ImGui in general seems to result in messy code
void Editor::update(sf::RenderWindow& window, EntityManager& entity_manager, GameEngine* engine) {
    ImGui::SFML::Update(window, m_dt.restart());
    ImGui::Begin("Editor");

    //  ImGui::ShowDemoWindow();
    
    if (ImGui::BeginTabBar("EditTabBar", 0)) {
        if (ImGui::BeginTabItem("Add")) {
            ImGui::SeparatorText("Add New Entity");
            auto pos = engine->selectedPos();
            if (pos == Vec2(0, 0)) {
                ImGui::Text("Click and select grid position");
            } else {
                ImGui::Text("Add entity to position: (%.0f, %0.f)", pos.x, pos.y);
            }
            if (ImGui::Button("Create")) {
                addEntity(entity_manager, engine);
                entity_manager.update();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Edit", (bool *)__null, editTabFlag())) {
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
                            e->addComponent<CBBox>(Vec2(64, 64));
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Animation")) {
                        if (e->hasComponent<CAnimation>()) {
                            auto anims = engine->assets().getAnimations();
                            const char* animations[anims.size()];
                            const auto anim_name = e->getComponent<CAnimation>().animation.getName();
                            static auto cur_index = 1;
                            auto i = 0;

                            for (const auto& pair : anims) {
                                animations[i] = pair.first.c_str();
                                if (pair.first == anim_name) {
                                    cur_index = i;
                                }
                                i++;
                            }

                            auto prev_index = cur_index;
                            ImGui::ListBox("Animations", &cur_index, animations, IM_ARRAYSIZE(animations), 6);
                            if (prev_index != cur_index) {
                                e->addComponent<CAnimation>(engine->assets().getAnimation(animations[cur_index]), true);
                                m_previous_animation = animations[cur_index];
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
                    if (ImGui::TreeNode("Behavior")) {
                        if (e->tag() == Tag::ENEMY) {
                            if (e->hasComponent<CBehavior>()) {
                                auto& hostile = e->getComponent<CBehavior>().hostile;
                                ImGui::Checkbox("Hostile", &hostile);
                            }
                            if (ImGui::TreeNode("Follow")) {
                                if (e->hasComponent<CFollowPlayer>()) {
                                    auto& follow = e->getComponent<CFollowPlayer>();
                                    ImGui::SetNextItemWidth(100);
                                    ImGui::InputFloat("x", &follow.home.x, 1.0f, 1.0f, "%.0f");
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(100);
                                    ImGui::InputFloat("y", &follow.home.y, 1.0f, 1.0f, "%.0f");
                                    ImGui::SetNextItemWidth(100);
                                    ImGui::InputFloat("Speed", &follow.speed, 1.0f, 1.0f, "%.0f");
                                } else if (ImGui::Button("Add follow behavior")) {
                                    e->addComponent<CFollowPlayer>();
                                    e->removeComponent<CPatrol>();
                                }
                                ImGui::TreePop();
                            }
                        }
                        if (e->tag() == Tag::ENEMY || e->tag() == Tag::ELEVATOR) {
                            if (ImGui::TreeNode("Patrol")) {
                                if (e->hasComponent<CPatrol>()) {
                                    auto& patrol = e->getComponent<CPatrol>();
                                    ImGui::SetNextItemWidth(100);
                                    ImGui::InputFloat("Speed", &patrol.speed, 1.0f, 1.0f, "%.0f");
                                    auto i = 0;
                                    for (auto& pos : patrol.positions) {
                                        ImGui::PushID(i);
                                        ImGui::SetNextItemWidth(100);
                                        ImGui::InputFloat("x", &pos.x, 1.0f, 1.0f, "%.0f");
                                        ImGui::SameLine();
                                        ImGui::SetNextItemWidth(100);
                                        ImGui::InputFloat("y", &pos.y, 1.0f, 1.0f, "%.0f");
                                        ImGui::SetNextItemWidth(100);
                                        ImGui::PopID();
                                        i++;
                                    }
                                    if (ImGui::Button("Add position")) {
                                        patrol.positions.push_back(Vec2(0.0f, 0.0f));
                                    }
                                    if (patrol.positions.size() > 2) {
                                        if (ImGui::Button("Pop position")) {
                                            patrol.positions.pop_back();
                                            patrol.cur_pos = 0;
                                        }
                                    }
                                } else if (ImGui::Button("Add follow behavior")) {
                                    e->addComponent<CPatrol>();
                                    e->removeComponent<CFollowPlayer>();
                                }
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Type")) {
                        static auto cur_index = 0;
                        for (auto i = 0; i < IM_ARRAYSIZE(m_types); i++) {
                            const std::string type_str(m_types[i]);
                            if (!m_type_map.count(type_str)) {
                                continue;
                            }
                            if (m_type_map.at(type_str) == e->tag()) {
                                cur_index = i;
                                break;
                            }
                        }
                        auto prev_index = cur_index;
                        ImGui::ListBox("Types", &cur_index, m_types, IM_ARRAYSIZE(m_types), 6);
                        if (prev_index != cur_index) {
                            std::string type(m_types[cur_index]);
                            if (m_type_map.count(type)) {
                                e->setTag(m_type_map.at(type));
                            }
                        }
                        ImGui::TreePop();
                    }
            
                    if (ImGui::Button("Delete")) {
                        e->destroy();
                        entity_manager.update();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Save")) {
                        parseEntities(entity_manager, engine);
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

void Editor::addEntity(EntityManager& entity_manager, GameEngine* engine) {
    auto tile = entity_manager.addEntity(Tag::TILE);
    tile->addComponent<CAnimation>(engine->assets().getAnimation(m_previous_animation), true);
    tile->addComponent<CTransform>(engine->selectedPos());
    tile->addComponent<CDraggable>();
    engine->setSelectedEntityId(tile->id());

    m_previously_created = true;
}

bool Editor::windowActive() const {
    auto& io = ImGui::GetIO();
	return io.WantCaptureMouse || io.WantCaptureKeyboard;
}

int Editor::editTabFlag() {
    // For ImGui automatic tab change
    if (m_previously_created) {
        m_previously_created = false;
        return 2;
    }
    return 0;
}

void Editor::setLevel(const std::string& level_path) {
    m_level_path = level_path;
}

const std::string Editor::tagString(Tag tag) const {
    std::map<Tag, std::string> tag_map = {
        { Tag::TILE, "Tile" },
        { Tag::PLAYER, "Player" },
        { Tag::ENEMY, "NPC" },
        { Tag::ELEVATOR, "Elevator" },
        { Tag::CHECKPOINT, "Checkpoint" },
        { Tag::HEART, "Tile" },
        { Tag::TELEPORT, "Tile" }
    };
    std::string tag_str = "";
    if (tag_map.count(tag)) {
        tag_str = tag_map.at(tag);
    }
    return tag_str;
}

void Editor::parseEntity(std::shared_ptr<Entity> e, GameEngine* engine) {
    const auto tag = tagString(e->tag());
    if (tag == "") {
        return;
    }

    if (!e->hasComponent<CTransform>()) {
        // Currently not saving entities without position to level files
        return;
    }

    const auto pos = e->getComponent<CTransform>().pos;
    const auto grid_pos = engine->toGridPos(Vec2(pos.x, pos.y));
    const auto grid_x = grid_pos.x;
    const auto grid_y = grid_pos.y;

    auto block_movement = false;
    auto block_vision = false;

    std::stringstream ss;

    if (e->hasComponent<CBBox>()) {
        const auto bbox = e->getComponent<CBBox>();
        block_movement = bbox.block_movement;
        block_vision = bbox.block_vision;

        if (tag == "Checkpoint") {
            ss << tag << " " << grid_x << " " << grid_y << " " << bbox.size.x << " " << bbox.size.y;
            files::addLine(m_level_content, ss.str());
            return;
        }
    }

    std::string animation_name =  "";
    if (e->hasComponent<CAnimation>()) {
        animation_name = e->getComponent<CAnimation>().animation.getName();
    }

    if (tag == "Player") {
        const auto cf = engine->getPlayerConfig();
        ss << tag << " " << grid_x << " " << grid_y << " " << cf.bbox_x << " " <<
            cf.bbox_y << " " << cf.v << " " << cf.max_v << " " << cf.jump_v << " " <<
            cf.gravity << " " << cf.weapon << " " << cf.hp;
        files::addLine(m_level_content, ss.str());
        return;
    }

    ss << tag << " " << animation_name << " " << grid_x << " " << grid_y << " " <<
        block_movement << " " << block_vision;
    
    if (tag == "NPC") {
        auto hostile = 0;
        auto hp = 1;
        auto damage = 0;

        if (e->hasComponent<CBehavior>()) {
            hostile = e->getComponent<CBehavior>().hostile;
        }
        if (e->hasComponent<CHealth>()) {
            hp = e->getComponent<CHealth>().current;
        }
        if (e->hasComponent<CDamage>()) {
            damage = e->getComponent<CDamage>().damage;
        }
        ss << " " << hostile << " " << hp << " " << damage;
    }

    if (e->hasComponent<CPatrol>()) {
        const auto patrol = e->getComponent<CPatrol>();
        ss << " Patrol " << patrol.speed << " " << patrol.positions.size();
        for (const auto& pos : patrol.positions) {
            const auto grid_pos = engine->toGridPos(pos);
            ss << " " << grid_pos.x << " " << grid_pos.y;
        }
    }

    if (e->hasComponent<CFollowPlayer>()) {
        const auto follow = e->getComponent<CFollowPlayer>();
        const auto grid_pos = engine->toGridPos(follow.home);
        ss << " Follow " << follow.speed << " " << grid_pos.x << " " << grid_pos.y; 
    }

    files::addLine(m_level_content, ss.str());
}

void Editor::parseEntities(EntityManager& entity_manager, GameEngine* engine) {
    m_level_content.clear();

    m_level_content.push_back("# Background: layer name");
    for (const auto& layer_name : engine->layerNames()) {
        m_level_content.push_back("Background " + layer_name);
    }

    m_level_content.push_back("\n# Tile: animation name, x, y, block movement, block vision");
    for (const auto& e : entity_manager.getEntities(Tag::TILE)) {
        parseEntity(e, engine);
    }
    for (const auto& e : entity_manager.getEntities(Tag::HEART)) {
        parseEntity(e, engine);
    }
    for (const auto& e : entity_manager.getEntities(Tag::TELEPORT)) {
        parseEntity(e, engine);
    }

    m_level_content.push_back("\n# Player: x, y, bboxw, bboxh, speed, max speed, jump speed, gravity, weapon animation name hp");
    for (const auto& e : entity_manager.getEntities(Tag::PLAYER)) {
        parseEntity(e, engine);
    }

    m_level_content.push_back("\n# NPC: animation name, x, y, block movement, block vision, hostile, hp, damage, behavior");
    for (const auto& e : entity_manager.getEntities(Tag::ENEMY)) {
        parseEntity(e, engine);
    }

    m_level_content.push_back("\n# Elevator: animation name, x, y, block movement, block vision, behavior");
    for (const auto& e : entity_manager.getEntities(Tag::ELEVATOR)) {
        parseEntity(e, engine);
    }

    m_level_content.push_back("\n# Checkpoint: x, y, bboxw, bboxh");
    for (const auto& e : entity_manager.getEntities(Tag::CHECKPOINT)) {
        parseEntity(e, engine);
    }
 
    files::writeFile(m_level_path, m_level_content);
}
