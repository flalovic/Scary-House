//
// Created by filip on 8/1/26.
//

#ifndef MATF_RG_PROJECT_GUICONTROLER_HPP
#define MATF_RG_PROJECT_GUICONTROLER_HPP
#include <engine/core/Controller.hpp>

namespace app {
    class GUIController : public engine::core::Controller {
        void initialize() override;
        void poll_events() override;
        void draw() override;
    };
}

#endif //MATF_RG_PROJECT_GUICONTROLER_HPP
