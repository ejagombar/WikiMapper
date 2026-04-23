#include "gui.hpp"
#include <atomic>
#include <cmath>
#include <imgui.h>
#include <mutex>
#include <string>

#include "../../lib/ImSearch/imsearch.hpp"
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
    style.WindowBorderSize = 1.0f;
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

    auto &searchStyle = ImSearch::GetStyle();
    searchStyle.Colors[ImSearchCol_TextHighlightedBg] = {0.0f, 0.0f, 0.0f, 0.0f};
    searchStyle.Colors[ImSearchCol_TextHighlighted] = ColorScheme::Accent;
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

void GUI::loadIconTextures() {
    try {
        m_graphIconTexture = LoadTexture("graph_icon.png");
        m_diceIconTexture = LoadTexture("dice_icon.png");
        m_backgroundIconTexture = LoadTexture("background.png");
        m_nodeIconTexture = LoadTexture("fa-file.png");
        m_edgeIconTexture = LoadTexture("fa-link.png");
        m_dbIconTexture = LoadTexture("fa-database.png");
    } catch (const std::exception &e) {
        globalLogger->error("Failed to load icon textures: ", e.what());
        m_graphIconTexture = 0;
        m_diceIconTexture = 0;
        m_backgroundIconTexture = 0;
        m_nodeIconTexture = 0;
        m_edgeIconTexture = 0;
        m_dbIconTexture = 0;
    }
}

GUI::GUI(GLFWwindow *m_window, std::string font, ControlPlane &controlData) : m_controlData(controlData) {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImSearch::CreateContext();
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
    ImSearch::DestroyContext();
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

    ImVec2 settingsSize(1060, 901);

    ImVec2 settingsPos = ImVec2(mainViewport->Pos.x + (mainViewport->Size.x - settingsSize.x) * 0.5f + 5,
                                mainViewport->Pos.y + (mainViewport->Size.y - settingsSize.y) * 0.5f + 27);
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
    ImGui::Checkbox("Size Nodes by Link Count", &m_controlData.engine.sizeByDegree);
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Spacing();

    separator();
    subtitle("Graph Data Source");

    ImGui::PushItemWidth(500.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);

    std::lock_guard<std::mutex> lock(m_controlData.app.dataSourceMutex);

    // Data source type dropdown
    int currentSourceType = static_cast<int>(m_controlData.app.dataSource.sourceType);
    const char *sourceTypes[] = {"HTTP Server", "Database (Neo4j)"};

    ImGui::Text("Data Source");
    ImGui::PushItemWidth(180.0f);
    if (ImGui::Combo("##sourceType", &currentSourceType, sourceTypes, IM_ARRAYSIZE(sourceTypes))) {
        m_controlData.app.dataSource.sourceType = static_cast<dbInterfaceType>(currentSourceType);
    }

    ImGui::Spacing();

    const int boxWidth = 280;

    if (m_controlData.app.dataSource.sourceType == dbInterfaceType::DB) {
        ImGui::Text("URL");
        ImGui::PushItemWidth(boxWidth);
        {
            std::string urlBuffer = m_controlData.app.dataSource.dbUrl;
            char buffer[256];
            strncpy(buffer, urlBuffer.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';
            if (ImGui::InputTextWithHint("##neo4jUrl", "bolt://localhost", buffer, IM_ARRAYSIZE(buffer))) {
                m_controlData.app.dataSource.dbUrl = buffer;
            }
        }
        ImGui::PopItemWidth();

        ImGui::Text("Password");
        ImGui::PushItemWidth(boxWidth);
        {
            char buffer[256];
            strncpy(buffer, m_controlData.app.dataSource.dbPassword.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';
            if (ImGui::InputTextWithHint("##neo4jPassword", "Enter password", buffer, IM_ARRAYSIZE(buffer),
                                         ImGuiInputTextFlags_Password)) {
                m_controlData.app.dataSource.dbPassword = buffer;
            }
        }
        ImGui::PopItemWidth();

    } else if (m_controlData.app.dataSource.sourceType == dbInterfaceType::HTTP) {
        ImGui::Text("URL");
        ImGui::PushItemWidth(boxWidth);
        {
            char buffer[256];
            strncpy(buffer, m_controlData.app.dataSource.serverUrl.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';
            if (ImGui::InputTextWithHint("##httpUrl", "http://eagombar.uk", buffer, IM_ARRAYSIZE(buffer))) {
                m_controlData.app.dataSource.serverUrl = buffer;
            }
        }
        ImGui::PopItemWidth();
    }

    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::Primary);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::PrimaryHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);

    if (ImGui::Button("Save & Connect", ImVec2(205, 50))) {
        m_controlData.app.dataSource.attemptDataConnection = true;
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
    ImVec4 statusColor = m_controlData.app.dataSource.connectedToDataSource ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                                                                            : ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

    drawList->AddCircleFilled(circlePos, 8.0f, ImGui::ColorConvertFloat4ToU32(statusColor));

    // Status text
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 25);
    ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
    ImGui::Text("%s", m_controlData.app.dataSource.connectedToDataSource ? "Connected" : "Disconnected");
    ImGui::PopStyleColor();

    ImGui::PopStyleColor();
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Spacing();

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

        boxPos = ImVec2(mainViewport->Pos.x, mainViewport->Pos.y + mainViewport->Size.y - boxSize.y - 20);
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

void GUI::RenderTopBar() {
    ImGuiViewport *vp = ImGui::GetMainViewport();
    const float frameH = ImGui::GetFrameHeight();
    const float barH = frameH + 20.0f;
    const float btnGap = 8.0f;
    const float searchWidth = 350.0f;

    // Panel width: slider(280) + inner spacing + longest label + window padding on both sides.
    // AlwaysAutoResize handles height; SizeConstraints fixes width so labels are never clipped.
    const float panelSliderW = 280.0f;
    const float panelPad = 14.0f;
    const float panelW = panelSliderW + ImGui::GetStyle().ItemInnerSpacing.x +
                         ImGui::CalcTextSize("Repulsion Strength").x + 2.0f * panelPad;

    ImGui::SetNextWindowViewport(vp->ID);
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, barH), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 10.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Surface);

    ImGui::Begin("##TopBar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);

    // Bottom border
    {
        ImVec2 wp = ImGui::GetWindowPos();
        ImGui::GetWindowDrawList()->AddLine(ImVec2(wp.x, wp.y + barH - 1.0f),
                                           ImVec2(wp.x + vp->Size.x, wp.y + barH - 1.0f),
                                           ImGui::ColorConvertFloat4ToU32(ColorScheme::Border), 1.0f);
    }

    auto VSep = [&]() {
        ImGui::SameLine(0, 12.0f);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + 0.5f, p.y), ImVec2(p.x + 0.5f, p.y + frameH),
                                           ImGui::ColorConvertFloat4ToU32(ColorScheme::Separator), 1.0f);
        ImGui::Dummy(ImVec2(1.0f, frameH));
        ImGui::SameLine(0, 12.0f);
    };

    // ── Physics / Rendering toggle buttons ────────────────────────────────────
    static bool s_physicsOpen = false;
    static bool s_renderingOpen = false;
    ImVec2 physicsPanelPos, renderingPanelPos;
    const float barBottom = ImGui::GetWindowPos().y + barH;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, s_physicsOpen ? ColorScheme::Primary : ColorScheme::SurfaceLight);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::PrimaryHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);
    if (ImGui::Button("Physics")) {
        s_physicsOpen = !s_physicsOpen;
        if (s_physicsOpen) s_renderingOpen = false;
    }
    physicsPanelPos = ImVec2(ImGui::GetItemRectMin().x, barBottom + 2.0f);
    ImGui::PopStyleColor(3);

    ImGui::SameLine(0, btnGap);

    ImGui::PushStyleColor(ImGuiCol_Button, s_renderingOpen ? ColorScheme::Primary : ColorScheme::SurfaceLight);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::PrimaryHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);
    if (ImGui::Button("Rendering")) {
        s_renderingOpen = !s_renderingOpen;
        if (s_renderingOpen) s_physicsOpen = false;
    }
    renderingPanelPos = ImVec2(ImGui::GetItemRectMin().x, barBottom + 2.0f);
    ImGui::PopStyleColor(3);

    ImGui::PopStyleVar();

    VSep();

    // ── Search bar ────────────────────────────────────────────────────────────
    // ImSearch::SearchBar calls SetNextItemWidth(-FLT_MIN) internally, so we wrap
    // it in a child window to constrain the width.
    ImVec4 searchBg = ColorScheme::SurfaceLight;
    if (m_controlData.graph.searching.load(std::memory_order_relaxed)) {
        float pulse = (sinf(m_settings.searchTimeElapsed * 4.0f) * 0.5f + 0.5f) * 0.35f;
        searchBg = ImVec4(ColorScheme::Primary.x * pulse + searchBg.x * (1.0f - pulse),
                          ColorScheme::Primary.y * pulse + searchBg.y * (1.0f - pulse),
                          ColorScheme::Primary.z * pulse + searchBg.z * (1.0f - pulse), 1.0f);
        m_settings.searchTimeElapsed += ImGui::GetIO().DeltaTime * 3.0f;
    } else {
        m_settings.searchTimeElapsed = 0.0f;
    }

    ImVec2 searchScreenPos = ImGui::GetCursorScreenPos();

    static bool suggestionsVisible = false;
    static std::string s_cachedUserQuery;
    static std::string s_pendingSetQuery;
    bool itemEdited = false, itemSubmitted = false, itemClicked = false;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, searchBg);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, searchBg);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.22f, 0.22f, 0.24f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
    if (ImGui::BeginChild("##searchbox", ImVec2(searchWidth, frameH), false,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        if (ImSearch::BeginSearch()) {
            if (!s_pendingSetQuery.empty()) {
                ImSearch::SetUserQuery(s_pendingSetQuery.c_str());
                s_pendingSetQuery.clear();
            }
            ImSearch::SearchBar("Search Wikipedia...");
            itemSubmitted = ImGui::IsItemDeactivatedAfterEdit();
            itemEdited = ImGui::IsItemEdited();
            itemClicked = ImGui::IsItemClicked();
            s_cachedUserQuery = ImSearch::GetUserQuery();
            ImSearch::EndSearch();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);

    if (itemSubmitted) {
        std::string query;
        if (m_controlData.app.searchSuggestionsMutex.try_lock()) {
            if (!m_controlData.app.searchSuggestions.empty()) {
                query = m_controlData.app.searchSuggestions.front();
                s_pendingSetQuery = query;
            }
            m_controlData.app.searchSuggestionsMutex.unlock();
        }
        if (query.empty())
            query = s_cachedUserQuery;
        if (!query.empty()) {
            m_controlData.graph.searchString = query;
            m_controlData.graph.searching.store(true);
        }
        suggestionsVisible = false;
    }

    if (itemEdited || itemClicked)
        suggestionsVisible = true;

    if (itemEdited) {
        std::lock_guard<std::mutex> lock(m_controlData.graph.searchStringMutex);
        m_controlData.graph.searchString = s_cachedUserQuery;
    }

    // ── Action buttons ────────────────────────────────────────────────────────
    ImGui::SameLine(0, btnGap);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::SurfaceLight);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::Primary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);

    ImGui::ImageButton("##graph", (ImTextureID)m_graphIconTexture, ImVec2(32, 32));
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Shuffle Layout");

    ImGui::SameLine(0, btnGap);
    if (ImGui::ImageButton("##dice", (ImTextureID)m_diceIconTexture, ImVec2(32, 32)))
        m_controlData.graph.addRandomPage.store(true, std::memory_order_relaxed);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Add Random Page");

    ImGui::SameLine(0, btnGap);
    if (ImGui::ImageButton("##background", (ImTextureID)m_backgroundIconTexture, ImVec2(32, 32)))
        m_controlData.engine.backgroundButtonToggle = !m_controlData.engine.backgroundButtonToggle;
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Cycle Background");

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // ── Stats (far right) ────────────────────────────────────────────────────
    {
        bool connected = m_controlData.app.dataSource.connectedToDataSource;
        int32_t nodes = m_controlData.engine.nodeCount.load(std::memory_order_relaxed);
        int32_t edges = m_controlData.engine.edgeCount.load(std::memory_order_relaxed);
        float simFPS = m_controlData.engine.simulationFPS.load(std::memory_order_relaxed);

        // Icon size matches text line height so no vertical adjustment is needed
        const float iconSz = ImGui::GetTextLineHeight();
        const float sep = 16.0f;  // gap between stat groups
        const float iconTextGap = 6.0f;
        const ImVec4 mutedTint(0.7f, 0.7f, 0.73f, 1.0f);
        const ImVec4 dbTint = connected ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.3f, 0.3f, 1.0f);

        char nodeBuf[32], edgeBuf[32], simBuf[32], fpsBuf[32];
        const char *dbLabel = connected ? "Connected" : "Disconnected";
        snprintf(nodeBuf, sizeof(nodeBuf), "%d", nodes);
        snprintf(edgeBuf, sizeof(edgeBuf), "%d", edges);
        snprintf(simBuf, sizeof(simBuf), "sim %.0f fps", static_cast<double>(simFPS));
        snprintf(fpsBuf, sizeof(fpsBuf), "%.0f fps", static_cast<double>(ImGui::GetIO().Framerate));

        // Measure total width for right-alignment
        auto statW = [&](const char *label) {
            return iconSz + iconTextGap + ImGui::CalcTextSize(label).x;
        };
        float totalW = statW(dbLabel) + sep + statW(nodeBuf) + sep + statW(edgeBuf) + sep +
                       ImGui::CalcTextSize(simBuf).x;
        if (m_settings.showFPS)
            totalW += sep + ImGui::CalcTextSize(fpsBuf).x;
        totalW += ImGui::GetStyle().WindowPadding.x;

        ImGui::SameLine(ImGui::GetWindowWidth() - totalW);

        // Draw icon + label, then advance with a separator gap
        auto IconStat = [&](GLuint tex, ImVec4 tint, const char *label) {
            ImGui::ImageWithBg((ImTextureID)(uintptr_t)tex, ImVec2(iconSz, iconSz),
                               ImVec2(0,0), ImVec2(1,1), ImVec4(0,0,0,0), tint);
            ImGui::SameLine(0, iconTextGap);
            ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextMuted);
            ImGui::Text("%s", label);
            ImGui::PopStyleColor();
            ImGui::SameLine(0, sep);
        };

        IconStat(m_dbIconTexture,   dbTint,    dbLabel);
        IconStat(m_nodeIconTexture, mutedTint, nodeBuf);
        IconStat(m_edgeIconTexture, mutedTint, edgeBuf);

        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextMuted);
        ImGui::Text("%s", simBuf);
        if (m_settings.showFPS) {
            ImGui::SameLine(0, sep);
            ImGui::Text("%s", fpsBuf);
        }
        ImGui::PopStyleColor();
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);

    // ── Physics panel (docked, persistent) ───────────────────────────────────
    if (s_physicsOpen) {
        ImGui::SetNextWindowViewport(vp->ID);
        ImGui::SetNextWindowPos(physicsPanelPos, ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(ImVec2(panelW, 0.0f), ImVec2(panelW, FLT_MAX));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(panelPad, 12.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Background);
        ImGui::PushStyleColor(ImGuiCol_Border, ColorScheme::Border);
        ImGui::Begin("##physics_panel", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing);

        ImGui::PushItemWidth(panelSliderW);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);

        subtitle("Physics");

        auto localSim = m_controlData.sim.parameters.load(std::memory_order_relaxed);
        bool updated[7] = {};

        ImGui::Text("Forces");
        ImGui::Separator();
        updated[0] = ImGui::SliderFloat("Repulsion Strength", &localSim.repulsionStrength, 0.01f, 100.f, "%.3f");
        updated[1] = ImGui::SliderFloat("Attraction Strength", &localSim.attractionStrength, 0.01f, 100.0f, "%.3f");
        updated[2] = ImGui::SliderFloat("Centering Force", &localSim.centeringForce, 0.1f, 50.0f, "%.3f");
        updated[5] = ImGui::SliderFloat("Max Force", &localSim.maxForce, 0.1f, 200.0f, "%.3f");

        ImGui::Spacing();
        ImGui::Text("Simulation");
        ImGui::Separator();
        updated[3] = ImGui::SliderFloat("Time Step", &localSim.timeStep, .01f, 1000.0f, "%.4f");
        updated[4] = ImGui::SliderFloat("Force Multiplier", &localSim.forceMultiplier, 0.1f, 10.0f, "%.3f");
        updated[6] = ImGui::SliderFloat("Target Distance", &localSim.targetDistance, .01f, 100.0f, "%.3f");

        if (updated[0] || updated[1] || updated[2] || updated[3] || updated[4] || updated[5] || updated[6])
            m_controlData.sim.parameters.store(localSim, std::memory_order_relaxed);

        ImGui::Spacing();
        ImGui::Text("Cooling");
        ImGui::Separator();
        ImGui::Checkbox("Enable Cooling", &m_controlData.engine.enableCooling);
        if (m_controlData.engine.enableCooling)
            ImGui::SliderFloat("Cooling Rate", &m_controlData.engine.coolingRate, 0.01f, 5.0f, "%.2f");

        ImGui::PopStyleColor();
        ImGui::PopItemWidth();
        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }

    // ── Rendering panel (docked, persistent) ─────────────────────────────────
    if (s_renderingOpen) {
        ImGui::SetNextWindowViewport(vp->ID);
        ImGui::SetNextWindowPos(renderingPanelPos, ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(ImVec2(panelW, 0.0f), ImVec2(panelW, FLT_MAX));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(panelPad, 12.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Background);
        ImGui::PushStyleColor(ImGuiCol_Border, ColorScheme::Border);
        ImGui::Begin("##rendering_panel", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing);

        ImGui::PushItemWidth(panelSliderW);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);

        subtitle("Rendering");

        auto &cv = m_controlData.engine.customVals;

        ImGui::Text("Lighting");
        ImGui::Separator();
        ImGui::SliderFloat("Specular Strength", &cv[0], 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess", &cv[1], 0.0f, 512.0f, "%.1f");
        ImGui::SliderFloat("Ambient", &cv[2], 0.0f, 1.0f, "%.3f");

        ImGui::Spacing();
        ImGui::Text("Camera");
        ImGui::Separator();
        ImGui::SliderFloat("Movement Speed", &m_controlData.engine.cameraMovementSpeed, 0.1f, 10.0f, "%.2f");

        ImGui::Spacing();
        ImGui::Text("Nodes");
        ImGui::Separator();
        ImGui::SliderFloat("Node Scale", &m_controlData.engine.nodeSizeMultiplier, 0.1f, 20.0f, "%.2f");
        ImGui::SliderInt("Search Result Limit", &m_controlData.engine.searchResultLimit, 0, 2000,
                         m_controlData.engine.searchResultLimit == 0 ? "unlimited" : "%d");

        ImGui::Spacing();
        ImGui::Text("Labels");
        ImGui::Separator();
        ImGui::SliderFloat("Label Scale", &m_controlData.engine.labelSizeMultiplier, 0.1f, 10.0f, "%.2f");
        ImGui::SliderFloat("Label Distance", &m_controlData.engine.labelDistanceThreshold, 10.0f, 500.0f, "%.0f");
        ImGui::SliderInt("Max Labels", &m_controlData.engine.maxLabelCount, 10, 1000);

        ImGui::PopStyleColor();
        ImGui::PopItemWidth();
        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }

    // ── Suggestions (floating window below search input) ──────────────────────
    if (suggestionsVisible) {
        bool hasSuggestions = false;
        if (m_controlData.app.searchSuggestionsMutex.try_lock()) {
            hasSuggestions = !m_controlData.app.searchSuggestions.empty();
            m_controlData.app.searchSuggestionsMutex.unlock();
        }

        if (hasSuggestions) {
            ImGui::SetNextWindowViewport(vp->ID);
            ImGui::SetNextWindowPos(ImVec2(searchScreenPos.x, searchScreenPos.y + frameH + 2.0f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(searchWidth, 0.0f), ImGuiCond_Always);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Surface);
            ImGui::PushStyleColor(ImGuiCol_Border, ColorScheme::Border);
            ImGui::Begin("##suggestions", nullptr,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoDocking);

            if (m_controlData.app.searchSuggestionsMutex.try_lock()) {
                for (const auto &s : m_controlData.app.searchSuggestions) {
                    if (s.empty())
                        continue;
                    if (ImGui::Selectable(s.c_str())) {
                        ImSearch::SetUserQuery(s.c_str());
                        m_controlData.graph.searchString = s;
                        m_controlData.graph.searching.store(true);
                        suggestionsVisible = false;
                    }
                }
                m_controlData.app.searchSuggestionsMutex.unlock();
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowHovered() &&
                !ImGui::IsMouseHoveringRect(searchScreenPos,
                                            ImVec2(searchScreenPos.x + searchWidth, searchScreenPos.y + frameH),
                                            false)) {
                suggestionsVisible = false;
            }

            ImGui::End();
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(2);
        } else {
            suggestionsVisible = false;
        }
    }
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
