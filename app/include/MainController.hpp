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
    void draw_tunnel();
    void draw() override;
    void end_draw() override;
};

#endif //MATF_RG_PROJECT_MAINCONTROLER_HPP
