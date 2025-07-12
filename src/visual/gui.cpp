#include "gui.hpp"
#include <atomic>
#include <cmath>
#include <imgui.h>
#include <iostream>
#include <string>
#include <sys/ucontext.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace ColorScheme {
const ImVec4 Background = ImVec4(0.04f, 0.04f, 0.05f, 0.97f);
const ImVec4 Surface = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
const ImVec4 SurfaceLight = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
const ImVec4 Primary = ImVec4(0.16f, 0.38f, 0.60f, 1.0f);
const ImVec4 PrimaryHover = ImVec4(0.20f, 0.46f, 0.70f, 1.0f);
const ImVec4 PrimaryActive = ImVec4(0.13f, 0.33f, 0.52f, 1.0f);
const ImVec4 Accent = ImVec4(0.60f, 0.30f, 0.55f, 1.0f);
const ImVec4 TextPrimary = ImVec4(0.94f, 0.94f, 0.95f, 1.0f);
const ImVec4 TextSecondary = ImVec4(0.70f, 0.70f, 0.73f, 1.0f);
const ImVec4 TextMuted = ImVec4(0.50f, 0.50f, 0.53f, 1.0f);
const ImVec4 Border = ImVec4(0.18f, 0.18f, 0.20f, 1.0f);
const ImVec4 Separator = ImVec4(0.28f, 0.28f, 0.30f, 1.0f);
} // namespace ColorScheme

std::string getPlatformOpenURLCmd(std::string url) {
#ifdef _WIN32
    return "start \"\" \"" + url + "\"";
#elif __APPLE__
    return "open \"" + url + "\"";
#else
    return "xdg-open \"" + url + "\"";
#endif
}

void GUI::setupTheme() {
    ImGuiStyle &style = ImGui::GetStyle();

    // Rounding
    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 8.0f;

    // Spacing
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(10, 8);
    style.ItemSpacing = ImVec2(10, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.IndentSpacing = 24.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;

    // Borders
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;

    // Colors
    ImVec4 *colors = style.Colors;
    colors[ImGuiCol_Text] = ColorScheme::TextPrimary;
    colors[ImGuiCol_TextDisabled] = ColorScheme::TextMuted;
    colors[ImGuiCol_WindowBg] = ColorScheme::Background;
    colors[ImGuiCol_ChildBg] = ColorScheme::Surface;
    colors[ImGuiCol_PopupBg] = ColorScheme::Surface;
    colors[ImGuiCol_Border] = ColorScheme::Border;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_FrameBg] = ColorScheme::SurfaceLight;
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
    colors[ImGuiCol_TitleBg] = ColorScheme::Surface;
    colors[ImGuiCol_TitleBgActive] = ColorScheme::Surface;
    colors[ImGuiCol_TitleBgCollapsed] = ColorScheme::Surface;
    colors[ImGuiCol_MenuBarBg] = ColorScheme::Surface;
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_CheckMark] = ColorScheme::Primary;
    colors[ImGuiCol_SliderGrab] = ColorScheme::Primary;
    colors[ImGuiCol_SliderGrabActive] = ColorScheme::PrimaryActive;
    colors[ImGuiCol_Button] = ColorScheme::Primary;
    colors[ImGuiCol_ButtonHovered] = ColorScheme::PrimaryHover;
    colors[ImGuiCol_ButtonActive] = ColorScheme::PrimaryActive;
    colors[ImGuiCol_Header] = ColorScheme::Primary;
    colors[ImGuiCol_HeaderHovered] = ColorScheme::PrimaryHover;
    colors[ImGuiCol_HeaderActive] = ColorScheme::PrimaryActive;
    colors[ImGuiCol_Separator] = ColorScheme::Separator;
    colors[ImGuiCol_SeparatorHovered] = ColorScheme::Primary;
    colors[ImGuiCol_SeparatorActive] = ColorScheme::PrimaryActive;
}

void GUI::subtitle(const char *text) {
    ImGui::PushFont(m_subTitleFont);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextPrimary);
    ImGui::Text("%s", text);
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Spacing();
}

void GUI::separator() {
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Separator, ColorScheme::Separator);
    ImGui::Separator();
    ImGui::PopStyleColor();
    ImGui::Spacing();
}

void GUI::SetActiveNodeInfo(std::string activeNodeTitle) { m_activeNodeTitle = activeNodeTitle; }

void GUI::SetOriginNodeInfo(std::string originNodeTitle) { m_originNodeTitle = originNodeTitle; }

void GUI::RenderFPSWidget() {
    if (!m_settings.showFPS)
        return;

    ImGuiViewport *mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(mainViewport->ID);

    ImVec2 widgetSize = ImVec2(98, 68);
    ImVec2 widgetPos = ImVec2(mainViewport->Pos.x + mainViewport->Size.x - widgetSize.x - 25, mainViewport->Pos.y + 25);

    ImGui::SetNextWindowPos(widgetPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(widgetSize, ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Surface);
    ImGui::PushStyleColor(ImGuiCol_Border, ColorScheme::Border);

    ImGui::Begin("##FPSWidget", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);

    float fps = ImGui::GetIO().Framerate;
    ImGui::PushFont(m_subTitleFont);
    ImGui::SetCursorPos(ImVec2(12, 13));
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextMuted);
    ImGui::Text("%.0f", fps);
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

void GUI::loadIconTextures() {
    try {
        m_graphIconTexture = LoadTexture("graph_icon.png");
        m_diceIconTexture = LoadTexture("dice_icon.png");
        m_backgroundIconTexture = LoadTexture("background.png");
    } catch (const std::exception &e) {
        globalLogger->error("Failed to load icon textures: ", e.what());
        m_graphIconTexture = 0;
        m_diceIconTexture = 0;
        m_backgroundIconTexture = 0;
    }
}

GUI::GUI(GLFWwindow *m_window, std::string font, ControlData &controlData) : m_controlData(controlData) {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    m_defaultFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 32.0f);
    m_titleFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 84.0f);
    m_subTitleFont = io.Fonts->AddFontFromFileTTF(font.c_str(), 42.0f);

    // io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    loadIconTextures();
    setupTheme();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

bool GUI::Active() {

    // if (m_overrideActive) {
    //     return false;
    // }

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
    ImGuiViewport *mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(mainViewport->ID);

    ImVec2 settingsSize(1050, 875);

    ImVec2 settingsPos = ImVec2(mainViewport->Pos.x + (mainViewport->Size.x - settingsSize.x) * 0.5f,
                                mainViewport->Pos.y + (mainViewport->Size.y - settingsSize.y) * 0.5f);
    ImGui::SetNextWindowPos(settingsPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(settingsSize, ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(40, 30));
    ImGui::Begin("Settings", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);

    ImGui::PushFont(m_titleFont);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextPrimary);
    float titleWidth = ImGui::CalcTextSize("WikiMapper").x;
    ImGui::SetCursorPosX((settingsSize.x - titleWidth) * 0.5f);
    ImGui::Text("WikiMapper");
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImVec2 contentSize = ImVec2(settingsSize.x - 80, settingsSize.y - 200);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0)); // Fully transparent
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::BeginChild("ContentCard", contentSize, false); // No border

    subtitle("About");
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + contentSize.x - 80);
    ImGui::TextWrapped("WikiMapper is an interactive explorer for traversing Wikipedia through a graph view. Each node "
                       "represents a page while the edges represent links between pages.");
    ImGui::PopTextWrapPos();
    ImGui::PopStyleColor();

    separator();

    // Camera Settings Section
    subtitle("Camera Settings");
    ImGui::PushItemWidth(350.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);

    ImGui::Text("Mouse Sensitivity");
    ImGui::SliderFloat("##mouseSens", &m_controlData.engine.mouseSensitivity, 0.1f, 10.0f, "%.1f",
                       ImGuiSliderFlags_AlwaysClamp);

    ImGui::Spacing();
    ImGui::Text("Field of View");
    ImGui::SliderFloat("##fov", &m_controlData.engine.cameraFov, 30.0f, 120.0f, "%.1f°", ImGuiSliderFlags_AlwaysClamp);

    ImGui::PopStyleColor();
    ImGui::PopItemWidth();

    separator();

    subtitle("Options");

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    ImGui::Checkbox("V-Sync", &m_controlData.engine.vSync);
    ImGui::Checkbox("Show FPS", &m_settings.showFPS);
    ImGui::Checkbox("Debug Controls", &m_settings.debugMode);
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

    if (ImGui::Button("Reset Simulation", ImVec2(205, 50))) {
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    separator();
    subtitle("Graph Data Source");

    ImGui::PushItemWidth(350.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);

    // Data source type dropdown
    ImGui::Text("Data Source Type");
    const char *sourceTypes[] = {"Neo4j Database", "HTTP Server"};
    int currentSourceType = static_cast<int>(m_dataSourceSettings.sourceType);
    if (ImGui::Combo("##dataSourceType", &currentSourceType, sourceTypes, IM_ARRAYSIZE(sourceTypes))) {
        m_dataSourceSettings.sourceType = static_cast<DataSourceType>(currentSourceType);
    }

    ImGui::Spacing();

    const int boxWidth = 280;

    if (m_dataSourceSettings.sourceType == DataSourceType::Neo4j) {
        ImGui::Text("URL");
        ImGui::PushItemWidth(boxWidth);
        ImGui::InputTextWithHint("##neo4jUrl", "http://localhost", m_dataSourceSettings.neo4jUrl,
                                 IM_ARRAYSIZE(m_dataSourceSettings.neo4jUrl));
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Text("Port");
        ImGui::SameLine();
        ImGui::PushItemWidth(80.0f);
        ImGui::InputTextWithHint("##neo4jPort", "7474", m_dataSourceSettings.neo4jPort,
                                 IM_ARRAYSIZE(m_dataSourceSettings.neo4jPort), ImGuiInputTextFlags_CharsDecimal);
        ImGui::PopItemWidth();

        ImGui::Text("Password");
        ImGui::PushItemWidth(boxWidth);
        ImGui::InputTextWithHint("##neo4jPassword", "Enter password", m_dataSourceSettings.neo4jPassword,
                                 IM_ARRAYSIZE(m_dataSourceSettings.neo4jPassword), ImGuiInputTextFlags_Password);
        ImGui::PopItemWidth();

    } else if (m_dataSourceSettings.sourceType == DataSourceType::HTTPServer) {
        ImGui::Text("URL");
        ImGui::PushItemWidth(boxWidth);
        ImGui::InputTextWithHint("##httpUrl", "http://eagombar.uk", m_dataSourceSettings.httpUrl,
                                 IM_ARRAYSIZE(m_dataSourceSettings.httpUrl));
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Text("Port");
        ImGui::SameLine();
        ImGui::PushItemWidth(80.0f);
        ImGui::InputTextWithHint("##httpPort", "6348", m_dataSourceSettings.httpPort,
                                 IM_ARRAYSIZE(m_dataSourceSettings.httpPort), ImGuiInputTextFlags_CharsDecimal);
        ImGui::PopItemWidth();
    }

    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::Primary);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::PrimaryHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);

    if (ImGui::Button("Save & Connect", ImVec2(205, 50))) {
        m_dataSourceSettings.connectionValid = !m_dataSourceSettings.connectionValid;
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();

    ImVec2 circlePos = ImGui::GetCursorScreenPos();
    circlePos.x += 10;
    circlePos.y += 25;

    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec4 statusColor =
        m_dataSourceSettings.connectionValid ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

    drawList->AddCircleFilled(circlePos, 8.0f, ImGui::ColorConvertFloat4ToU32(statusColor));

    // Status text
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 25);
    ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
    ImGui::Text("%s", m_dataSourceSettings.connectionValid ? "Connected" : "Disconnected");
    ImGui::PopStyleColor();

    ImGui::PopStyleColor();
    ImGui::PopItemWidth();

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::End();
}

void GUI::RenderBottomLeftBox() {
    ImGuiViewport *mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(mainViewport->ID);

    m_overrideActive = false;
    ImVec2 boxSize = ImVec2(850, 115);
    ImVec2 localPos = ImVec2(0, ImGui::GetIO().DisplaySize.y - boxSize.y);
    ImVec2 boxPos = ImVec2(mainViewport->Pos.x + localPos.x, mainViewport->Pos.y + localPos.y);
    ImGui::SetNextWindowPos(boxPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(boxSize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::Begin("##bottomLeftBox", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);

    if (!m_activeNodeTitle.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextMuted);
        ImGui::Text("Node:");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextPrimary);
        ImGui::Text("%s", m_activeNodeTitle.empty() ? "None" : m_activeNodeTitle.c_str());
        ImGui::PopStyleColor();
    }
    ImGui::End();

    if (!m_activeNodeTitle.empty()) {
        m_overrideActive = true;
        boxSize = ImVec2(250, 50);

        ImVec2 boxPos = ImVec2(mainViewport->Pos.x, mainViewport->Pos.y + mainViewport->Size.y - boxSize.y - 20);
        ImGui::SetNextWindowPos(boxPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(boxSize);

        ImGui::Begin("##bottomLeftBox2", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::Primary);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        std::string linkText = "Open on Wikipedia";
        if (ImGui::Button(linkText.c_str())) {
            std::string url = "https://en.wikipedia.org/wiki/" + m_activeNodeTitle;
            system(getPlatformOpenURLCmd(url).c_str());
        }
        if (ImGui::IsItemHovered()) {
            m_overrideActive = false;
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::End();
    }
    ImGui::PopStyleColor(2);
}

void GUI::RenderSearchBar() {
    ImGuiViewport *mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(mainViewport->ID);

    ImVec2 searchBarSize = ImVec2(585, 68);
    ImVec2 localPos = ImVec2(25, 25);
    ImVec2 searchBarPos = ImVec2(mainViewport->Pos.x + localPos.x, mainViewport->Pos.y + localPos.y);

    // glow effect for searching
    ImVec4 searchBarColor = ColorScheme::Surface;
    if (m_controlData.graph.searching.load(std::memory_order_relaxed)) {
        float pulse = (sin(m_settings.searchTimeElapsed * 2.0f) * 0.5f + 0.5f) * 0.3f;
        searchBarColor = ImVec4(ColorScheme::Primary.x * pulse + searchBarColor.x * (1.0f - pulse),
                                ColorScheme::Primary.y * pulse + searchBarColor.y * (1.0f - pulse),
                                ColorScheme::Primary.z * pulse + searchBarColor.z * (1.0f - pulse), 0.95f);
        m_settings.searchTimeElapsed += ImGui::GetIO().DeltaTime * 3.0f;
    }

    ImGui::SetNextWindowPos(searchBarPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(searchBarSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, searchBarColor);
    ImGui::PushStyleColor(ImGuiCol_Border, ColorScheme::Border);

    ImGui::Begin("##searchbar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);

    // Search input
    ImGui::PushItemWidth(450);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ColorScheme::SurfaceLight);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextPrimary);

    static char searchBuffer[128] = "";
    bool enterPressed = ImGui::InputTextWithHint("##searchInput", "Search Wikipedia...", searchBuffer,
                                                 IM_ARRAYSIZE(searchBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
    m_controlData.graph.searchString = std::string(searchBuffer);

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::Primary);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::PrimaryHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);

    if (enterPressed || ImGui::Button("Search", ImVec2(100, 48))) {
        m_controlData.graph.searching.store(true);
        m_settings.searchTimeElapsed = 0.0f;
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::End();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(2);

    ImVec2 actionPanelSize = ImVec2(126 + 68, 68);
    ImVec2 actionPanelPos = ImVec2(searchBarPos.x + searchBarSize.x + 15, searchBarPos.y);

    ImGui::SetNextWindowPos(actionPanelPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(actionPanelSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Surface);
    ImGui::PushStyleColor(ImGuiCol_Border, ColorScheme::Border);

    ImGui::Begin("##actionpanel", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::SurfaceLight);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::Primary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);

    float buttonSize = 48.0f;
    float spacing = 10.0f;

    float totalWidth = (buttonSize * 3) + spacing * 3;
    float startX = (actionPanelSize.x - totalWidth) * 0.5f;
    ImGui::SetCursorPosX(startX);

    if (ImGui::ImageButton("##graph", (ImTextureID)m_graphIconTexture, ImVec2(32, 32))) {
        std::cout << "Graph overview button clicked!" << std::endl;
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shuffle Layout");
    }

    ImGui::SameLine(0, spacing);

    if (ImGui::ImageButton("##dice", (ImTextureID)m_diceIconTexture, ImVec2(32, 32))) {
        std::cout << "Graph overview button clicked!" << std::endl;
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Add Random Page");
    }

    ImGui::SameLine(0, spacing);

    if (ImGui::ImageButton("##background", (ImTextureID)m_backgroundIconTexture, ImVec2(32, 32))) {
        std::cout << "Graph overview button clicked!" << std::endl;
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Cycle Background");
    }

    ImGui::PopStyleColor(3);
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
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Background);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);

    ImGui::Begin("Debug Controls", &m_settings.debugMode, ImGuiWindowFlags_None);

    ImGui::PushItemWidth(300.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);

    SimulationControlData localSim = m_controlData.sim.load(std::memory_order_relaxed);

    if (ImGui::CollapsingHeader("Physics Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        bool valUpdated[7];

        ImGui::Text("Force Settings");
        ImGui::Separator();
        valUpdated[0] = ImGui::SliderFloat("Repulsion Strength", &localSim.repulsionStrength, 0.01f, 100.f, "%.3f");
        valUpdated[1] = ImGui::SliderFloat("Attraction Strength", &localSim.attractionStrength, 0.01f, 100.0f, "%.3f");
        valUpdated[2] = ImGui::SliderFloat("Centering Force", &localSim.centeringForce, 0.1f, 50.0f, "%.3f");
        valUpdated[5] = ImGui::SliderFloat("Max Force", &localSim.maxForce, 0.1f, 200.0f, "%.3f");

        ImGui::Spacing();
        ImGui::Text("Simulation Settings");
        ImGui::Separator();
        valUpdated[3] = ImGui::SliderFloat("Time Step", &localSim.timeStep, .01f, 10.0f, "%.3f");
        valUpdated[4] = ImGui::SliderFloat("Force Multiplier", &localSim.forceMultiplier, 0.1f, 10.0f, "%.3f");
        valUpdated[6] = ImGui::SliderFloat("Target Distance", &localSim.targetDistance, .01f, 100.0f, "%.3f");

        if (valUpdated[0] | valUpdated[1] | valUpdated[2] | valUpdated[3] | valUpdated[4] | valUpdated[5] |
            valUpdated[6]) {
            m_controlData.sim.store(localSim, std::memory_order_relaxed);
        }
    }

    if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto &colors = m_controlData.engine.customVals;

        ImGui::Text("Lighting");
        ImGui::Separator();
        ImGui::SliderFloat("Specular Strength", &colors[0], 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess", &colors[1], 0.0f, 512.0f, "%.1f");
        ImGui::SliderFloat("Ambient", &colors[2], 0.0f, 1.0f, "%.3f");
    }

    ImGui::PopStyleColor();
    ImGui::PopItemWidth();
    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}
