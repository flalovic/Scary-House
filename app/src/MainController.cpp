//
// Created by filip on 8/1/26.
//

#include "../include/MainController.hpp"

#include <GUIControler.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <glm/gtc/matrix_transform.hpp>

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->set_enable_cursor(false);
}

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MainController::update() {
    auto gui_controller = engine::core::Controller::get<app::GUIController>();;
    if (!gui_controller->is_enabled()) {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        float dt = platform->dt();
        if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed) {
            camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed) {
            camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed) {
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed) {
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }
        auto mouse = platform->mouse();
        camera->rotate_camera(mouse.dx, mouse.dy);
        camera->zoom(mouse.scroll);
    }
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    // draw
    draw_space();
}

void MainController::draw_space() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    auto shader = resources->shader("advanced");
    auto house = resources->model("scary");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model(1.0f);
    shader->set_mat4("model", model);

    // Kamera
    shader->set_vec3("camPos", graphics->camera()->Position);

    // Spotlight (na kameri)
    shader->set_vec3("spotLightPos", graphics->camera()->Position);
    shader->set_vec3("spotLightDir", graphics->camera()->Front);

    shader->set_float("spotInnerCutOff", glm::cos(glm::radians(12.5f)));
    shader->set_float("spotOuterCutOff", glm::cos(glm::radians(17.5f)));

    house->draw(shader);
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}
