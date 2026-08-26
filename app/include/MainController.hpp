//
// Created by filip on 8/1/26.
//

#ifndef MATF_RG_PROJECT_MAINCONTROLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>

class MainController : public engine::core::Controller{
public:
    MainController();

    [[nodiscard]] bool is_spotlight_enabled() const {
        return m_spotlight_enabled;
    }

    void set_spotlight_enabled(bool enabled) {
        m_spotlight_enabled = enabled;
    }

    [[nodiscard]] const glm::vec3 &spotlight_diffuse_color() const {
        return m_spotlight_diffuse;
    }

    void set_spotlight_diffuse_color(const glm::vec3 &color) {
        m_spotlight_diffuse = color;
    }

    [[nodiscard]] const glm::vec3 &point_light_diffuse_color() const {
        return m_point_light_diffuse;
    }

    void set_point_light_diffuse_color(const glm::vec3 &color) {
        m_point_light_diffuse = color;
    }

private:
    void initialize() override;
    bool loop() override;
    void update() override;

    void begin_draw() override;
    void draw() override;
    void end_draw() override;

    void draw_space();
    void draw_pillar();
    void draw_lightbox();
    void draw_wizard();
    void draw_point_shadow_scene();

    enum class EventState { IDLE, WAITING_TO_SHOW, WAITING_TO_HIDE };
    EventState m_wizard_state = EventState::IDLE;
    float m_wizard_timer = 0.0f;
    bool m_show_wizard = false;
    bool m_spotlight_enabled = true;
    glm::vec3 m_spotlight_diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 m_point_light_position = glm::vec3(0.998914f, 1.6983163f, 1.201679f);
    glm::vec3 m_point_light_diffuse = glm::vec3(7.0f, 3.0f, 0.5f);
};

#endif //MATF_RG_PROJECT_MAINCONTROLER_HPP
