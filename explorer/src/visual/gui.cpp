#include "gui.hpp"
#include <imgui.h>

GUI::GUI(GLFWwindow *m_window, std::string font) {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    m_defaultFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 35.0f);
    m_titleFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 100.0f);

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

GUI::~GUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
};

void GUI::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::RenderMenu() {
    ImVec2 settingsSize(800, 800);
    ImVec2 settingsPos((ImGui::GetIO().DisplaySize.x - settingsSize.x) * 0.5f,
                       (ImGui::GetIO().DisplaySize.y - settingsSize.y) * 0.5f);
    ImGui::SetNextWindowPos(settingsPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(settingsSize, ImGuiCond_Always);
    ImGui::Begin("Settings", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);

    float windowWidth = ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f;

    ImGui::PushFont(m_titleFont);
    float titleWidth = ImGui::CalcTextSize("WikiMapper").x;
    ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
    ImGui::Text("WikiMapper");
    ImGui::PopFont();
    ImGui::Separator();

    for (uint i = 0; i < 20; i++) {
        // Controls Section
        float controlsWidth = ImGui::CalcTextSize("Controls").x;
        ImGui::SetCursorPosX((windowWidth - controlsWidth) * 0.5f);
        ImGui::Text("Controls");

        float widgetWidth = 300.0f;
        ImGui::PushItemWidth(widgetWidth);
        float centerX = (windowWidth - widgetWidth) * 0.5f;

        ImGui::SetCursorPosX(centerX);
        static float mouseSensitivity = 1.0f;
        ImGui::SliderFloat("Mouse Sensitivity", &mouseSensitivity, 0.1f, 10.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::SetCursorPosX(centerX);
        static float fov = 90.0f;
        ImGui::SliderFloat("FOV", &fov, 60.0f, 120.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::PopItemWidth();
    }

    ImGui::End();

    ImGui::Render();
};

void GUI::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::RenderOverlay() {};
