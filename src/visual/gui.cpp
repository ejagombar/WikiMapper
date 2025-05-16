#include "gui.hpp"
#include <atomic>
#include <cmath>
#include <imgui.h>
#include <string>
#include <sys/ucontext.h>

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

GUI::GUI(GLFWwindow *m_window, std::string font, ControlData &controlData) : m_controlData(controlData) {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 2.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    m_defaultFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 35.0f);
    m_titleFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 100.0f);
    m_subTitleFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 50.0f);

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

bool GUI::Active() {
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureMouse;
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
    float margin = 50.0f;

    // Title
    ImGui::PushFont(m_titleFont);
    float titleWidth = ImGui::CalcTextSize("WikiMapper").x;
    ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
    ImGui::Text("WikiMapper");
    ImGui::PopFont();

    // Separator with margins drawn manualy
    ImVec2 separatorStart = ImVec2(ImGui::GetWindowPos().x + margin, ImGui::GetCursorScreenPos().y);
    ImVec2 separatorEnd = ImVec2(ImGui::GetWindowPos().x + windowWidth - margin, separatorStart.y);
    ImGui::GetWindowDrawList()->AddLine(separatorStart, separatorEnd, IM_COL32(255, 255, 255, 255), 1.5f);
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    ImVec2 childSize = ImVec2(settingsSize.x - margin * 2, settingsSize.y - 150);
    ImGui::SetCursorPosX(margin);
    ImGui::BeginChild("ScrollableArea", childSize, false); // No border

    // --------------------------------------------------

    subtitle("About");
    ImGui::TextWrapped("WikiMapper is an interactive explorer used to traverse the Wikipedia link tree. Each node "
                       "represents a pagewhilst the edges of the graph represent the links between pages. Although "
                       "links are unidirectional, that is not being shown at the moment. This text is really just "
                       "filler so I can test the scroll bar.");

    separator();
    subtitle("Camera Settings");
    ImGui::PushItemWidth(300.0f);
    ImGui::SliderFloat("Mouse Sensitivity", &m_controlData.engine.mouseSensitivity, 0.1f, 10.0f, "%.1f",
                       ImGuiSliderFlags_AlwaysClamp);

    ImGui::SliderFloat("FOV", &m_controlData.engine.cameraFov, 30.0f, 120.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::PopItemWidth();

    separator();
    subtitle("Options");

    ImGui::Checkbox("Debug Controls", &m_settings.debugMode);
    ImGui::Checkbox("V-Sync", &m_controlData.engine.vSync);
    ImGui::Button("Reset Simulation");

    ImGui::EndChild();
    ImGui::End();
};

void GUI::RenderBottomLeftBox() {
    ImVec2 boxSize = ImVec2(500, 110);
    ImVec2 boxPos = ImVec2(20, ImGui::GetIO().DisplaySize.y - boxSize.y - 20);
    ImVec4 boxColor = ImVec4(0.02f, 0.02f, 0.02f, 0.95f);

    ImGui::SetNextWindowPos(boxPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(boxSize);

    // Use WindowRounding for the outer window corners and no border
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 30.0f); // Increased rounding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(60, 50));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // No border outline
    ImGui::PushStyleColor(ImGuiCol_WindowBg, boxColor);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));

    ImGui::Begin("##bottomLeftBox", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoSavedSettings);

    // Render the fixed, non-scrollable top text line using a larger bold font
    ImGui::PushFont(m_subTitleFont);
    ImGui::Text(" Root: ");
    ImGui::PopFont();

    ImGui::Text(" Hovering: ");

    ImGui::End();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void GUI::RenderSearchBar() {
    ImVec2 searchBarSize = ImVec2(500, 100);
    ImVec2 searchBarPos = ImVec2(20, 0);

    ImVec4 searchBarColor = ImVec4(0.02f, 0.02f, 0.02f, 0.95f);
    if (m_controlData.graph.searching.load(std::memory_order_relaxed)) {
        float pulse = (sin(m_settings.searchTimeElapsed * 0.75f) * 0.5f + 0.5f) * 0.1f + 0.02f;
        searchBarColor = ImVec4(pulse, pulse, pulse, 0.95f);
        m_settings.searchTimeElapsed += ImGui::GetIO().DeltaTime * 4.0f;
    }

    ImGui::SetNextWindowPos(searchBarPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(searchBarSize);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 40.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, searchBarColor);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImGui::Begin("##searchbar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoSavedSettings);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));
    ImGui::SetCursorPos(ImVec2(10, 35));
    ImGui::PushItemWidth(350);

    // TODO: Get InputText box working with std::string
    static char searchBuffer[128] = "";
    bool enterPressed = ImGui::InputTextWithHint("##searchInput", "Search", searchBuffer, IM_ARRAYSIZE(searchBuffer),
                                                 ImGuiInputTextFlags_EnterReturnsTrue);
    m_controlData.graph.searchString = std::string(searchBuffer);

    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, searchBarColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(searchBarColor.x + 0.1f, searchBarColor.y + 0.1f, searchBarColor.z + 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, searchBarColor);

    if (enterPressed || ImGui::Button("Go", ImVec2(80, 50))) {
        m_controlData.graph.searching.store(true);
        m_settings.searchTimeElapsed = 0.0f;
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar();
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

void GUI::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void GUI::RenderDebugMenu() {
    ImGui::Begin("Debug", nullptr);

    ImGui::PushItemWidth(500.0f);

    SimulationControlData localSim = m_controlData.sim.load(std::memory_order_relaxed);

    bool valUpdated[10];
    valUpdated[0] = ImGui::SliderFloat("Repulsion Strength", &localSim.repulsionStrength, 1.0f, 1000.f, "%.3f",
                                       ImGuiSliderFlags_AlwaysClamp);
    valUpdated[1] = ImGui::SliderFloat("Attraction Strength", &localSim.attractionStrength, 0.02f, 20, "%.3f",
                                       ImGuiSliderFlags_AlwaysClamp);
    valUpdated[2] = ImGui::SliderFloat("Centering Force", &localSim.centeringForce, 10.0f, 10000.0f, "%.3f",
                                       ImGuiSliderFlags_AlwaysClamp);
    valUpdated[3] =
        ImGui::SliderFloat("Time Step", &localSim.timeStep, .01f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valUpdated[4] = ImGui::SliderFloat("Force Multiplier", &localSim.forceMultiplier, 0.1f, 10.0f, "%.3f",
                                       ImGuiSliderFlags_AlwaysClamp);
    valUpdated[5] =
        ImGui::SliderFloat("Max Force", &localSim.maxForce, 0.1f, 20.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valUpdated[6] = ImGui::SliderFloat("Target Distance", &localSim.targetDistance, .01f, 10.0f, "%.3f",
                                       ImGuiSliderFlags_AlwaysClamp);

    if (valUpdated[0] | valUpdated[1] | valUpdated[2] | valUpdated[3] | valUpdated[4] | valUpdated[5] | valUpdated[6]) {
        m_controlData.sim.store(localSim, std::memory_order_relaxed);
    }

    auto &colors = m_controlData.engine.customColors;

    ImGui::SliderFloat("Color 1", &colors[0], 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderFloat("Color 2", &colors[1], 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderFloat("Color 3", &colors[2], 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderFloat("Color 4", &colors[3], 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderFloat("Color 5", &colors[4], 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    ImGui::PopItemWidth();
    ImGui::End();
};
