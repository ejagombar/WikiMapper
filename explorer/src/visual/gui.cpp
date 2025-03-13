#include "gui.hpp"
#include <imgui.h>
#include <string>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void GUI::subtitle(const char *text) {
    ImGui::PushFont(m_subTitleFont);
    ImGui::Text("%s", text);
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
}

void GUI::separator() {
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
}

GUI::GUI(GLFWwindow *m_window, std::string font) {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    m_defaultFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 35.0f);
    m_titleFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 100.0f);
    m_subTitleFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 50.0f);

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
    ImVec2 settingsSize(1000, 800);
    ImVec2 settingsPos((ImGui::GetIO().DisplaySize.x - settingsSize.x) * 0.5f,
                       (ImGui::GetIO().DisplaySize.y - settingsSize.y) * 0.5f);
    ImGui::SetNextWindowPos(settingsPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(settingsSize, ImGuiCond_Always);
    ImGui::Begin("Settings", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);

    float windowWidth = ImGui::GetWindowSize().x;
    float margin = 50.0f; // Margin for separator and content

    // Title
    ImGui::PushFont(m_titleFont);
    float titleWidth = ImGui::CalcTextSize("WikiMapper").x;
    ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
    ImGui::Text("WikiMapper");
    ImGui::PopFont();

    // Separator with Margins (Manually Drawn)
    ImVec2 separatorStart = ImVec2(ImGui::GetWindowPos().x + margin, ImGui::GetCursorScreenPos().y);
    ImVec2 separatorEnd = ImVec2(ImGui::GetWindowPos().x + windowWidth - margin, separatorStart.y);
    ImGui::GetWindowDrawList()->AddLine(separatorStart, separatorEnd, IM_COL32(255, 255, 255, 255), 1.5f);
    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Add spacing after separator

    ImVec2 childSize = ImVec2(settingsSize.x - margin * 2, settingsSize.y - 150);
    ImGui::SetCursorPosX(margin);
    ImGui::BeginChild("ScrollableArea", childSize, false, ImGuiWindowFlags_AlwaysVerticalScrollbar); // No border

    // --------------------------------------------------

    subtitle("About");
    ImGui::TextWrapped("WikiMapper is an interactive explorer used to traverse the Wikipedia link tree. Each node "
                       "represents a pagewhilst the edges of the graph represent the links between pages. Although "
                       "links are unidirectional, that is not being shown at the moment. This text is really just "
                       "filler so I can test the scroll bar.");

    separator();
    subtitle("Camera Settings");
    ImGui::PushItemWidth(300.0f);
    static float mouseSensitivity = 5.0f;
    ImGui::SliderFloat("Mouse Sensitivity", &mouseSensitivity, 0.1f, 10.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

    static float fov = 90.0f;
    ImGui::SliderFloat("FOV", &fov, 60.0f, 120.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::PopItemWidth();

    separator();
    subtitle("Options");

    static bool debugMode = true;
    static bool wireFrame = false;
    static bool vSync = true;
    ImGui::Checkbox("Debug Controls", &debugMode);
    ImGui::Checkbox("Wire Frame", &wireFrame);
    ImGui::Checkbox("V-Sync", &vSync);

    separator();
    subtitle("Help");
    ImGui::TextWrapped("Visit the Github for more information.");

    ImGui::EndChild();
    ImGui::End();
};

void GUI::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::RenderOverlay() {};
