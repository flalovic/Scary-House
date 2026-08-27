//
// Created by filip on 8/1/26.
//

#include <imgui.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUIControler.hpp"
#include "MainController.hpp"

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

    void GUIController::render_gui() {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto main_controller = engine::core::Controller::get<MainController>();
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

        bool spotlight_enabled = main_controller->is_spotlight_enabled();
        if (ImGui::Checkbox("Enable Spotlight", &spotlight_enabled)) {
            main_controller->set_spotlight_enabled(spotlight_enabled);
        }

        if (!spotlight_enabled) ImGui::BeginDisabled();
        glm::vec3 spotlight_diffuse = main_controller->spotlight_diffuse_color();
        if (ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(spotlight_diffuse))) {
            main_controller->set_spotlight_diffuse_color(spotlight_diffuse);
        }
        if (!spotlight_enabled) ImGui::EndDisabled();

        glm::vec3 point_light_diffuse = main_controller->point_light_diffuse_color();
        if (ImGui::ColorEdit3(
                "Point Light Diffuse",
                glm::value_ptr(point_light_diffuse),
                ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR
            )) {
            main_controller->set_point_light_diffuse_color(point_light_diffuse);
        }

        ImGui::Separator();
        ImGui::Text("Post-processing");

        bool bloom_enabled = graphics->bloom_enabled();
        if (ImGui::Checkbox("Enable Bloom", &bloom_enabled)) {
            graphics->enable_bloom(bloom_enabled);
        }

        bool point_shadows_enabled = graphics->point_shadows_enabled();
        if (ImGui::Checkbox("Enable Point Shadows", &point_shadows_enabled)) {
            graphics->enable_point_shadows(point_shadows_enabled);
        }

        float exposure = graphics->exposure();
        if (ImGui::SliderFloat("Exposure", &exposure, 0.0f, 1.0f, "%.2f")) {
            graphics->set_exposure(exposure);
        }

        ImGui::End();
        graphics->end_gui();
    }
}
