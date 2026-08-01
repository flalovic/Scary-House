//
// Created by filip on 8/1/26.
//

#ifndef MATF_RG_PROJECT_MAINCONTROLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLER_HPP
#include <engine/core/Controller.hpp>

class MainController : public engine::core::Controller{
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

    enum class EventState { IDLE, WAITING_TO_SHOW, WAITING_TO_HIDE };
    EventState m_wizard_state = EventState::IDLE;
    float m_wizard_timer = 0.0f;
    bool m_show_wizard = false;
};

#endif //MATF_RG_PROJECT_MAINCONTROLER_HPP
