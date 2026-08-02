//
// Created by filip on 8/1/26.
//

#ifndef MATF_RG_PROJECT_GUICONTROLER_HPP
#define MATF_RG_PROJECT_GUICONTROLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>

namespace app {
    class GUIController : public engine::core::Controller {
        void initialize() override;
        void poll_events() override;
        void draw() override;

        bool m_spotlight_enabled = true;
        glm::vec3 m_spotlight_diffuse = glm::vec3(0.8f, 0.8f, 0.8f);

    public:
        void render_gui() {
            draw();
        }

        [[nodiscard]] bool is_spotlight_enabled() const { return m_spotlight_enabled; }
        [[nodiscard]] const glm::vec3& spotlight_diffuse_color() const { return m_spotlight_diffuse; }
    };
}

#endif //MATF_RG_PROJECT_GUICONTROLER_HPP
