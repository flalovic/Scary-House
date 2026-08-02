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
        auto camera = graphics->camera();

        graphics->begin_gui();
        ImGui::SetNextWindowSize(ImVec2(420, 360), ImGuiCond_FirstUseEver);

        ImGui::Begin("Settings");

        // Kamera info
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);

        ImGui::Separator();
        ImGui::Text("Spotlight Controls");

        ImGui::Checkbox("Enable Spotlight", &m_spotlight_enabled);

        if (!m_spotlight_enabled) ImGui::BeginDisabled();
        ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(m_spotlight_diffuse));
        if (!m_spotlight_enabled) ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::Text("Post-processing");

        m_bloom_enabled = graphics->bloom_enabled();
        if (ImGui::Checkbox("Enable Bloom", &m_bloom_enabled)) {
            graphics->enable_bloom(m_bloom_enabled);
        }

        ImGui::SliderFloat("Exposure", &m_exposure, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemEdited()) {
            graphics->set_exposure(m_exposure);
        }

        ImGui::End();
        graphics->end_gui();
    }
}
