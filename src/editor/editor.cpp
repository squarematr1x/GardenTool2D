#include "editor.hpp"

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

void Editor::render(sf::RenderWindow& window) {
    ImGui::SFML::Update(window, m_dt.restart());

    ImGui::Begin("Editor");
    ImGui::SeparatorText("Add entity");
    // ImGui::Checkbox("Block vision", &block_vision);
    // ImGui::Checkbox("Block vision", &save);
    // ImGui::InputFloat("Val", &val);

    // if (ImGui::Button("Save")) {
    //     save = true;
    // }

    ImGui::ShowDemoWindow();
    ImGui::End();

    ImGui::SFML::Render(window);
}

void Editor::processEvent(const sf::RenderWindow& window, const sf::Event& event) {
    ImGui::SFML::ProcessEvent(window, event);
}

void Editor::addEntity(std::shared_ptr<Entity> e) {
    (void)e;
    // Store entity e data to file in m_level_path
}

void Editor::modifyEntity(std::shared_ptr<Entity> e) {
    (void)e;
    // Store entity e data to file in m_level_path
}

void Editor::deleteEntity(std::shared_ptr<Entity> e) {
    (void)e;
    // Store entity e data to file in m_level_path
}
