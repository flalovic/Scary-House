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

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->enable_bloom(true);
    graphics->set_exposure(0.2f);

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->set_enable_cursor(false);

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->set_pose(
        glm::vec3(-3.475257f, 1.576614f, 1.998679f),
        -13.900038f,
        -2.000012f
    );
}

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MainController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    auto gui_controller = engine::core::Controller::get<app::GUIController>();
    if (gui_controller->is_enabled()) {
        auto mouse = platform->mouse();
        camera->zoom(mouse.scroll);
        graphics->perspective_params().FOV = glm::radians(camera->Zoom);
        return;
    }

    if (platform->key(engine::platform::KEY_K).state() == engine::platform::Key::State::JustPressed) {
        if (m_wizard_state == EventState::IDLE) {
            m_wizard_state = EventState::WAITING_TO_SHOW;
            m_wizard_timer = 0.0f;
        }
    }

    if (m_wizard_state != EventState::IDLE) {
        m_wizard_timer += dt;

        // Nakon 2s -> Prikazi wizarda
        if (m_wizard_state == EventState::WAITING_TO_SHOW && m_wizard_timer >= 2.0f) {
            m_show_wizard = true;
            m_wizard_timer = 0.0f;
            m_wizard_state = EventState::WAITING_TO_HIDE;
        }
        // Nakon jos 2s -> Sakrij wizarda
        else if (m_wizard_state == EventState::WAITING_TO_HIDE && m_wizard_timer >= 2.0f) {
            m_show_wizard = false;
            m_wizard_state = EventState::IDLE;
        }
    }

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
    graphics->perspective_params().FOV = glm::radians(camera->Zoom);
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->begin_render();
}

void MainController::draw() {
    // draw
    draw_space();
    draw_pillar();
    draw_lightbox();

    if (m_show_wizard) {
        draw_wizard();
    }
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

    // Slanje komandi iz GUI-ja u shader
    auto gui_controller = engine::core::Controller::get<app::GUIController>();
    shader->set_bool("spotLightEnabled", gui_controller->is_spotlight_enabled());
    shader->set_vec3("spotLightDiffuse", gui_controller->spotlight_diffuse_color());

    house->draw(shader);
}

void MainController::draw_pillar() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto gui_controller = engine::core::Controller::get<app::GUIController>();

    auto shader = resources->shader("advanced");
    auto pillar = resources->model("pillar");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 0.05f, 1.2f));
    model = glm::scale(model, glm::vec3(0.0002f));
    shader->set_mat4("model", model);


    shader->set_vec3("camPos", graphics->camera()->Position);
    shader->set_vec3("spotLightPos", graphics->camera()->Position);
    shader->set_vec3("spotLightDir", graphics->camera()->Front);
    shader->set_float("spotInnerCutOff", glm::cos(glm::radians(12.5f)));
    shader->set_float("spotOuterCutOff", glm::cos(glm::radians(17.5f)));
    shader->set_bool("spotLightEnabled", gui_controller->is_spotlight_enabled());
    shader->set_vec3("spotLightDiffuse", gui_controller->spotlight_diffuse_color());

    pillar->draw(shader);
}

void MainController::draw_lightbox() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    auto shader = resources->shader("basic");
    auto light_box = resources->model("magma_ball");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    // glm::vec3 pillar_top = glm::vec3(0.998914f, 1.407529f, 1.201679f);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.9f, 1.407529f, 1.93f));
    model = glm::scale(model, glm::vec3(0.01f));

    shader->set_mat4("model", model);

    shader->set_vec3("lightColor", glm::vec3(1.0f, 0.9f, 0.7f));
    light_box->draw(shader);
}

void MainController::draw_wizard() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto gui_controller = engine::core::Controller::get<app::GUIController>();

    auto shader = resources->shader("advanced");
    auto wizard = resources->model("wizard");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 1.0f, 1.1f));
    model = glm::scale(model, glm::vec3(1.5f));
    shader->set_mat4("model", model);

    shader->set_vec3("camPos", graphics->camera()->Position);
    shader->set_vec3("spotLightPos", graphics->camera()->Position);
    shader->set_vec3("spotLightDir", graphics->camera()->Front);
    shader->set_float("spotInnerCutOff", glm::cos(glm::radians(12.5f)));
    shader->set_float("spotOuterCutOff", glm::cos(glm::radians(17.5f)));
    shader->set_bool("spotLightEnabled", gui_controller->is_spotlight_enabled());
    shader->set_vec3("spotLightDiffuse", gui_controller->spotlight_diffuse_color());

    wizard->draw(shader);
}

void MainController::end_draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->end_render();

    if (auto gui_controller = engine::core::Controller::get<app::GUIController>(); gui_controller->is_enabled()) {
        gui_controller->render_gui();
    }

    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}
