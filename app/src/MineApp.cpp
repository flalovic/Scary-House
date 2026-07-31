//
// Created by filip on 8/1/26.
//

#include "../include/MineApp.hpp"

#include <MainController.hpp>

namespace app {
    void MineApp::app_setup() {
        auto main_controller = register_controller<MainController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    }
} // app