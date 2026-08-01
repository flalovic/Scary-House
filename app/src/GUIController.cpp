//
// Created by filip on 8/1/26.
//

#include <imgui.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUIControler.hpp"

namespace app {
    void GUIController::initialize() {
        set_enable(false);
    }

    void GUIController::poll_events() {
        const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_F2).state() == engine::platform::Key::State::JustPressed) {
            platform->set_enable_cursor(!is_enabled());
            set_enable(!is_enabled());
        }
    }

    void GUIController::draw() {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

        graphics->begin_gui();
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

        ImGui::Begin("Camera info");

        // Kamera info
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);

        ImGui::Separator();
        ImGui::Text("Spotlight Controls");

        // Prekidač za uključivanje/isključivanje i birač boje
        ImGui::Checkbox("Enable Spotlight", &m_spotlight_enabled);

        // Opciono: onemogući biranje boje ako je spotlight isključen
        if (!m_spotlight_enabled) ImGui::BeginDisabled();
        ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(m_spotlight_diffuse));
        if (!m_spotlight_enabled) ImGui::EndDisabled();

        ImGui::End();
        graphics->end_gui();
    }
}
