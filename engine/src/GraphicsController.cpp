// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/resources/Skybox.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace engine::graphics {

void GraphicsController::initialize() {
    const int opengl_initialized = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    RG_GUARANTEE(opengl_initialized, "OpenGL failed to init!");

    auto platform = engine::core::Controller::get<platform::PlatformController>();
    auto handle = platform->window()->handle_();
    m_perspective_params.FOV = glm::radians(m_camera.Zoom);
    m_perspective_params.Width = static_cast<float>(platform->window()->width());
    m_perspective_params.Height = static_cast<float>(platform->window()->height());
    m_perspective_params.Near = 0.1f;
    m_perspective_params.Far = 100.f;
    m_ortho_params.Bottom = 0.0f;
    m_ortho_params.Top = static_cast<float>(platform->window()->height());
    m_ortho_params.Left = 0.0f;
    m_ortho_params.Right = static_cast<float>(platform->window()->width());
    m_ortho_params.Near = 0.1f;
    m_ortho_params.Far = 100.0f;

    platform->register_platform_event_observer(std::make_unique<GraphicsPlatformEventObserver>(this));
    CHECKED_GL_CALL(glViewport, 0, 0, platform->window()->width(), platform->window()->height());

    initialize_bloom();
    initialize_point_shadows();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    RG_GUARANTEE(ImGui_ImplGlfw_InitForOpenGL(handle, true), "ImGUI failed to initialize for OpenGL");
    RG_GUARANTEE(ImGui_ImplOpenGL3_Init("#version 330 core"), "ImGUI failed to initialize for OpenGL");
}

void GraphicsController::terminate() {
    destroy_point_shadows();
    destroy_bloom();

    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void GraphicsPlatformEventObserver::on_window_resize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    m_graphics->perspective_params().Width = static_cast<float>(width);
    m_graphics->perspective_params().Height = static_cast<float>(height);
    m_graphics->orthographic_params().Right = static_cast<float>(width);
    m_graphics->orthographic_params().Top = static_cast<float>(height);
    m_graphics->resize_bloom_buffers(width, height);
    CHECKED_GL_CALL(glViewport, 0, 0, width, height);
}

std::string_view GraphicsController::name() const {
    return "GraphicsController";
}

void GraphicsController::begin_gui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GraphicsController::end_gui() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsController::draw_skybox(const resources::Shader *shader, const resources::Skybox *skybox) {
    glm::mat4 view = glm::mat4(glm::mat3(m_camera.view_matrix()));
    shader->use();
    shader->set_mat4("view", view);
    shader->set_mat4("projection", projection_matrix<>());
    CHECKED_GL_CALL(glDepthFunc, GL_LEQUAL);
    CHECKED_GL_CALL(glBindVertexArray, skybox->vao());
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, skybox->texture());
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 36);
    CHECKED_GL_CALL(glBindVertexArray, 0);
    CHECKED_GL_CALL(glDepthFunc, GL_LESS);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, 0);
}

static void render_quad() {
    static GLuint quadVAO = 0;
    static GLuint quadVBO = 0;
    if (quadVAO == 0) {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        CHECKED_GL_CALL(glGenVertexArrays, 1, &quadVAO);
        CHECKED_GL_CALL(glGenBuffers, 1, &quadVBO);
        CHECKED_GL_CALL(glBindVertexArray, quadVAO);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, quadVBO);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    CHECKED_GL_CALL(glBindVertexArray, quadVAO);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void GraphicsController::initialize_bloom() {
    auto platform = engine::core::Controller::get<platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();

    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_hdr_fbo);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_fbo);

    CHECKED_GL_CALL(glGenTextures, 2, m_hdr_color_buffers);
    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_hdr_color_buffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_hdr_color_buffers[i], 0);
    }

    CHECKED_GL_CALL(glGenRenderbuffers, 1, &m_hdr_depth_rbo);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, m_hdr_depth_rbo);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_hdr_depth_rbo);

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);

    CHECKED_GL_CALL(glGenFramebuffers, 2, m_pingpong_fbo);
    CHECKED_GL_CALL(glGenTextures, 2, m_pingpong_color_buffers);

    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_pingpong_fbo[i]);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_pingpong_color_buffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpong_color_buffers[i], 0);
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void GraphicsController::destroy_bloom() {
    if (m_hdr_fbo) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_hdr_fbo);
        CHECKED_GL_CALL(glDeleteTextures, 2, m_hdr_color_buffers);
        CHECKED_GL_CALL(glDeleteRenderbuffers, 1, &m_hdr_depth_rbo);
        m_hdr_fbo = 0;
    }

    if (m_pingpong_fbo[0]) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 2, m_pingpong_fbo);
        CHECKED_GL_CALL(glDeleteTextures, 2, m_pingpong_color_buffers);
        m_pingpong_fbo[0] = m_pingpong_fbo[1] = 0;
    }
}

void GraphicsController::initialize_point_shadows() {
    int max_texture_units = 0;
    CHECKED_GL_CALL(glGetIntegerv, GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
    RG_GUARANTEE(max_texture_units > 0, "OpenGL reports no fragment shader texture units");
    m_point_shadow_texture_unit = max_texture_units - 1;

    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_point_shadow_fbo);
    CHECKED_GL_CALL(glGenTextures, 1, &m_point_shadow_depth_cubemap);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, m_point_shadow_depth_cubemap);

    for (unsigned int face = 0; face < 6; ++face) {
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT,
                        POINT_SHADOW_SIZE, POINT_SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_point_shadow_fbo);
    CHECKED_GL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_point_shadow_depth_cubemap, 0);
    CHECKED_GL_CALL(glDrawBuffer, GL_NONE);
    CHECKED_GL_CALL(glReadBuffer, GL_NONE);
    RG_GUARANTEE(CHECKED_GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                 "Point shadow framebuffer is incomplete");
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void GraphicsController::destroy_point_shadows() {
    if (m_point_shadow_fbo != 0) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_point_shadow_fbo);
        m_point_shadow_fbo = 0;
    }
    if (m_point_shadow_depth_cubemap != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 1, &m_point_shadow_depth_cubemap);
        m_point_shadow_depth_cubemap = 0;
    }
}

void GraphicsController::enable_point_shadows(bool enabled) {
    m_point_shadows_enabled = enabled;
}

bool GraphicsController::point_shadows_enabled() const {
    return m_point_shadows_enabled;
}

void GraphicsController::begin_point_shadow_pass(const glm::vec3 &light_position) {
    const glm::mat4 shadow_projection = glm::perspective(
        glm::radians(90.0f), 1.0f, POINT_SHADOW_NEAR_PLANE, POINT_SHADOW_FAR_PLANE);
    const glm::mat4 shadow_matrices[] = {
        shadow_projection * glm::lookAt(light_position, light_position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        shadow_projection * glm::lookAt(light_position, light_position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        shadow_projection * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        shadow_projection * glm::lookAt(light_position, light_position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        shadow_projection * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        shadow_projection * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("engine-point-shadow");
    shader->use();
    for (int face = 0; face < 6; ++face) {
        shader->set_mat4("shadowMatrices[" + std::to_string(face) + "]", shadow_matrices[face]);
    }
    shader->set_vec3("lightPos", light_position);
    shader->set_float("far_plane", POINT_SHADOW_FAR_PLANE);

    CHECKED_GL_CALL(glViewport, 0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_point_shadow_fbo);
    CHECKED_GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);
}

void GraphicsController::end_point_shadow_pass() {
    auto platform = engine::core::Controller::get<platform::PlatformController>();
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glViewport, 0, 0, platform->window()->width(), platform->window()->height());
}

void GraphicsController::bind_point_shadow_map(const resources::Shader *shader) const {
    shader->set_bool("pointShadows", m_point_shadows_enabled);
    shader->set_float("far_plane", POINT_SHADOW_FAR_PLANE);
    shader->set_int("pointShadowMap", m_point_shadow_texture_unit);
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0 + m_point_shadow_texture_unit);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, m_point_shadow_depth_cubemap);
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
}

void GraphicsController::resize_bloom_buffers(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    destroy_bloom();
    initialize_bloom();
}

void GraphicsController::enable_bloom(bool enabled) {
    m_bloom_enabled = enabled;
}

bool GraphicsController::bloom_enabled() const {
    return m_bloom_enabled;
}

void GraphicsController::set_exposure(float exposure) {
    m_exposure = exposure;
}

float GraphicsController::exposure() const {
    return m_exposure;
}

void GraphicsController::begin_render() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_fbo);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsController::end_render() {
    if (m_bloom_enabled) {
        blur_pass();
    }

    final_pass();
}

void GraphicsController::blur_pass() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shaderBlur = resources->shader("engine-blur");

    bool horizontal = true;
    bool first_iteration = true;
    unsigned int amount = 10;

    shaderBlur->use();

    for (unsigned int i = 0; i < amount; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_pingpong_fbo[horizontal]);
        shaderBlur->set_int("horizontal", horizontal);

        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D,
            first_iteration ? m_hdr_color_buffers[1] : m_pingpong_color_buffers[!horizontal]);

        render_quad();

        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void GraphicsController::final_pass() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shaderFinal = resources->shader("engine-bloom-final");

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderFinal->use();

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_hdr_color_buffers[0]);
    shaderFinal->set_int("scene", 0);

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_pingpong_color_buffers[0]);
    shaderFinal->set_int("bloomBlur", 1);

    shaderFinal->set_bool("bloom", m_bloom_enabled);
    shaderFinal->set_float("exposure", m_exposure);

    render_quad();
}

} // namespace engine::graphics