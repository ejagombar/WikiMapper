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
const ImVec4 Primary = ImVec4(0.0275f, 0.5059f, 0.7961f, 1.0f); // #0781cb
const ImVec4 PrimaryHover = ImVec4(0.0392f, 0.3019f, 0.9529f, 1.0f);
const ImVec4 PrimaryActive = ImVec4(0.0235f, 0.4314f, 0.6784f, 1.0f);
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
        m_eyeIconTexture = LoadTexture("fa-eye.png");
        m_atomIconTexture = LoadTexture("fa-atom.png");
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
    ImGui::SetNextWindowPos(mainViewport->Pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(mainViewport->Size, ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::Begin("##menuOverlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);

    ImGui::PushFont(m_titleFont);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextPrimary);
    const char *title = "II";
    ImVec2 titleSz = ImGui::CalcTextSize(title);
    ImGui::SetCursorPos(ImVec2((mainViewport->Size.x - titleSz.x) * 0.5f, (mainViewport->Size.y - titleSz.y) * 0.5f));
    ImGui::Text("%s", title);
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::End();
    ImGui::PopStyleColor(2);
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
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);

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
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

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

bool GUI::RenderTopBar() {
    ImGuiIO &io = ImGui::GetIO();
    bool interacted = io.MouseClicked[0] && io.WantCaptureMouse;

    ImGuiViewport *vp = ImGui::GetMainViewport();
    const float frameH = ImGui::GetFrameHeight();
    const float barH = frameH + 20.0f;
    const float btnGap = 8.0f;
    const float searchWidth = 350.0f;

    // Panel width: slider(280) + inner spacing + longest label + window padding on both sides.
    // AlwaysAutoResize handles height; SizeConstraints fixes width so labels are never clipped.
    const float panelSliderW = 280.0f;
    const float panelPad = 14.0f;
    const float panelW = panelSliderW + ImGui::GetStyle().ItemInnerSpacing.x + ImGui::CalcTextSize("Repulsion Strength").x + 2.0f * panelPad;

    ImGui::SetNextWindowViewport(vp->ID);
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, barH), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 10.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Surface);

    ImGui::Begin("##TopBar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);

    // Bottom border
    {
        ImVec2 wp = ImGui::GetWindowPos();
        ImGui::GetWindowDrawList()->AddLine(ImVec2(wp.x, wp.y + barH - 1.0f), ImVec2(wp.x + vp->Size.x, wp.y + barH - 1.0f), ImGui::ColorConvertFloat4ToU32(ColorScheme::Border), 1.0f);
    }

    auto VSep = [&]() {
        ImGui::SameLine(0, 12.0f);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + 0.5f, p.y), ImVec2(p.x + 0.5f, p.y + frameH), ImGui::ColorConvertFloat4ToU32(ColorScheme::Separator), 1.0f);
        ImGui::Dummy(ImVec2(1.0f, frameH));
        ImGui::SameLine(0, 12.0f);
    };

    // ── Physics / Rendering toggle buttons ────────────────────────────────────
    static bool s_physicsOpen = false;
    static bool s_renderingOpen = false;
    static bool s_dbOpen = false;
    static int s_highlightedIdx = -1;
    static std::vector<std::pair<std::string, std::string>> s_cachedSuggestions;
    static float s_physicsCloseTimer = 0.0f;
    static float s_renderingCloseTimer = 0.0f;
    static float s_dbCloseTimer = 0.0f;
    ImVec2 physicsPanelPos, renderingPanelPos, dbPanelPos;
    bool dbBtnHovered = false;
    const float barBottom = ImGui::GetWindowPos().y + barH;

    bool physicsTabHovered = false;
    bool renderingTabHovered = false;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    // Per-corner rounding requires drawing the bg manually before the Button call so
    // that the button text renders on top of our custom background.
    const float tabRounding = 6.0f;
    ImDrawList *dl = ImGui::GetWindowDrawList();
    const ImVec2 mousePos = ImGui::GetIO().MousePos;
    const ImGuiStyle &st = ImGui::GetStyle();

    // Physics tab — left corners rounded, right corners square
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 sz = ImGui::CalcTextSize("Physics");
        ImVec2 bMax = ImVec2(p.x + sz.x + st.FramePadding.x * 2, p.y + sz.y + st.FramePadding.y * 2);
        bool hov = mousePos.x >= p.x && mousePos.x <= bMax.x && mousePos.y >= p.y && mousePos.y <= bMax.y;
        ImU32 bg = ImGui::ColorConvertFloat4ToU32((s_physicsOpen || hov) ? ColorScheme::SurfaceLight : ImVec4(1, 1, 1, 0.05f));
        dl->AddRectFilled(p, bMax, bg, tabRounding, ImDrawFlags_RoundCornersLeft);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Text, s_physicsOpen ? ColorScheme::TextPrimary : ColorScheme::TextSecondary);
        ImGui::Button("Physics");
        physicsTabHovered = ImGui::IsItemHovered();
        if (physicsTabHovered) {
            s_physicsOpen = true;
            s_renderingOpen = false;
        }
        if (s_physicsOpen) {
            ImVec2 rmin = ImGui::GetItemRectMin();
            ImVec2 rmax = ImGui::GetItemRectMax();
            dl->AddRectFilled(ImVec2(rmin.x, rmax.y - 2.0f), rmax, ImGui::ColorConvertFloat4ToU32(ColorScheme::Primary));
        }
        physicsPanelPos = ImVec2(ImGui::GetItemRectMin().x, barBottom);
        ImGui::PopStyleColor(4);
    }

    ImGui::SameLine(0, 0.0f);

    // Rendering tab — right corners rounded, left corners square
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 sz = ImGui::CalcTextSize("Rendering");
        ImVec2 bMax = ImVec2(p.x + sz.x + st.FramePadding.x * 2, p.y + sz.y + st.FramePadding.y * 2);
        bool hov = mousePos.x >= p.x && mousePos.x <= bMax.x && mousePos.y >= p.y && mousePos.y <= bMax.y;
        ImU32 bg = ImGui::ColorConvertFloat4ToU32((s_renderingOpen || hov) ? ColorScheme::SurfaceLight : ImVec4(1, 1, 1, 0.05f));
        dl->AddRectFilled(p, bMax, bg, tabRounding, ImDrawFlags_RoundCornersRight);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Text, s_renderingOpen ? ColorScheme::TextPrimary : ColorScheme::TextSecondary);
        ImGui::Button("Rendering");
        renderingTabHovered = ImGui::IsItemHovered();
        if (renderingTabHovered) {
            s_renderingOpen = true;
            s_physicsOpen = false;
        }
        if (s_renderingOpen) {
            ImVec2 rmin = ImGui::GetItemRectMin();
            ImVec2 rmax = ImGui::GetItemRectMax();
            dl->AddRectFilled(ImVec2(rmin.x, rmax.y - 2.0f), rmax, ImGui::ColorConvertFloat4ToU32(ColorScheme::Primary));
        }
        renderingPanelPos = ImVec2(ImGui::GetItemRectMin().x, barBottom);
        ImGui::PopStyleColor(4);
    }

    ImGui::PopStyleVar(2);

    VSep();

    // ── Search bar ────────────────────────────────────────────────────────────
    // ImSearch::SearchBar calls SetNextItemWidth(-FLT_MIN) internally, so we wrap
    // it in a child window to constrain the width.
    ImVec4 searchBg = ColorScheme::SurfaceLight;
    if (m_controlData.graph.searching.load(std::memory_order_relaxed)) {
        float pulse = (sinf(m_settings.searchTimeElapsed * 4.0f) * 0.5f + 0.5f) * 0.35f;
        searchBg = ImVec4(ColorScheme::Primary.x * pulse + searchBg.x * (1.0f - pulse), ColorScheme::Primary.y * pulse + searchBg.y * (1.0f - pulse),
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
    if (ImGui::BeginChild("##searchbox", ImVec2(searchWidth, frameH), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
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

    // Clickable magnifying-glass region at the right edge of the search bar.
    // ImSearch draws a  incon there; we overlay an invisible button so clicks
    // on it trigger the search.
    {
        float iconW = ImGui::GetTextLineHeight();
        ImVec2 iconMin(searchScreenPos.x + searchWidth - iconW - 6.0f, searchScreenPos.y);
        ImVec2 iconMax(searchScreenPos.x + searchWidth, searchScreenPos.y + frameH);
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(iconMin, iconMax, false) && !s_cachedUserQuery.empty()) {
            std::string query = s_cachedUserQuery;
            {
                std::lock_guard<std::mutex> lock(m_controlData.graph.searchStringMutex);
                m_controlData.graph.searchString = query;
            }
            m_controlData.graph.searching.store(true);
            suggestionsVisible = false;
            s_highlightedIdx = -1;
            globalLogger->debug("[suggest] magnifying glass clicked — searching '{}'", query);
        }
    }

    bool enterPressed = ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter);

    if (itemSubmitted || enterPressed) {
        if (enterPressed && !itemSubmitted)
            globalLogger->debug("[suggest] Enter key fallback — itemSubmitted was false");
        if (itemSubmitted)
            globalLogger->debug("[suggest] itemSubmitted=true (ImGui deactivated after edit)");
        std::string query;
        // Use the arrow-key-highlighted suggestion if available.
        // Otherwise search for whatever the user actually typed — never
        // fall back to searchSuggestions.front() which may be stale.
        if (s_highlightedIdx >= 0 && s_highlightedIdx < (int)s_cachedSuggestions.size()) {
            query = s_cachedSuggestions[s_highlightedIdx].first;
            s_pendingSetQuery = s_cachedSuggestions[s_highlightedIdx].second;
            globalLogger->info("[suggest] Enter pressed — using highlighted idx {}: '{}'", s_highlightedIdx, query);
        } else {
            query = s_cachedUserQuery;
            s_pendingSetQuery = s_cachedUserQuery;
            globalLogger->debug("[suggest] Enter pressed — no highlight, using typed text '{}'", query);
        }
        if (!query.empty()) {
            {
                std::lock_guard<std::mutex> lock(m_controlData.graph.searchStringMutex);
                m_controlData.graph.searchString = query;
            }
            m_controlData.graph.searching.store(true);
        }
        suggestionsVisible = false;
        s_highlightedIdx = -1;
    }

    // Show suggestions on edit, click, or when the search bar gains focus.
    // Also fall back to the raw mouse-click rect in case the child window
    // intercepts IsItemClicked.
    if (itemEdited || itemClicked || ImGui::IsItemActivated() ||
        (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(searchScreenPos, ImVec2(searchScreenPos.x + searchWidth, searchScreenPos.y + frameH), false))) {
        if (!suggestionsVisible)
            globalLogger->debug("[suggest] autocomplete dropdown opened (edited={}, clicked={}, activated={})", itemEdited, itemClicked, (bool)ImGui::IsItemActivated());
        suggestionsVisible = true;
    }

    // Dismiss suggestions when the search bar loses focus
    if (suggestionsVisible && ImGui::IsItemDeactivated()) {
        suggestionsVisible = false;
        s_highlightedIdx = -1;
    }

    {
        std::lock_guard<std::mutex> lock(m_controlData.graph.searchStringMutex);
        if (m_controlData.graph.searchString != s_cachedUserQuery && !m_controlData.graph.searching.load()) {
            m_controlData.graph.searchString = s_cachedUserQuery;
            globalLogger->debug("[suggest] text changed — set searchString to '{}'", s_cachedUserQuery);
        }
    }

    // ── Action buttons ────────────────────────────────────────────────────────
    ImGui::SameLine(0, btnGap);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::SurfaceLight);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::Primary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);

    if (ImGui::ImageButton("##dice", (ImTextureID)m_diceIconTexture, ImVec2(32, 32)))
        m_controlData.graph.addRandomPage.store(true, std::memory_order_relaxed);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Add Random Page");

    ImGui::SameLine(0, btnGap);
    if (ImGui::ImageButton("##background", (ImTextureID)m_backgroundIconTexture, ImVec2(32, 32)))
        m_controlData.engine.backgroundButtonToggle = !m_controlData.engine.backgroundButtonToggle;
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Change Background");

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // ── Stats (far right) ────────────────────────────────────────────────────
    {
        bool connected = m_controlData.app.dataSource.connectedToDataSource;
        int32_t nodes = m_controlData.engine.nodeCount.load(std::memory_order_relaxed);
        int32_t edges = m_controlData.engine.edgeCount.load(std::memory_order_relaxed);
        float simFPS = m_controlData.engine.simulationFPS.load(std::memory_order_relaxed);

        const float iconSz = ImGui::GetTextLineHeight();
        const float sep = 14.0f;
        const float iconTextGap = 5.0f;
        const ImVec4 iconTint = ColorScheme::Primary;
        const ImVec4 dbTint = connected ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.3f, 0.3f, 1.0f);

        char nodeBuf[32], edgeBuf[32], simBuf[32], fpsBuf[32];
        snprintf(nodeBuf, sizeof(nodeBuf), "%d", nodes);
        snprintf(edgeBuf, sizeof(edgeBuf), "%d", edges);
        snprintf(simBuf, sizeof(simBuf), "%d fps", std::min(static_cast<int>(simFPS), 9999));
        snprintf(fpsBuf, sizeof(fpsBuf), "%d fps", std::min(static_cast<int>(ImGui::GetIO().Framerate), 9999));

        const float fpsIconSz = iconSz * 0.75f;
        float fpsFixedTextW = ImGui::CalcTextSize("9999 fps").x;
        float fpsGroupW = fpsIconSz + iconTextGap + fpsFixedTextW;
        float fpsSectionW = m_settings.showFPS ? (sep + fpsGroupW) : 0.0f;

        float totalW =
            iconSz + sep + iconSz + iconTextGap + ImGui::CalcTextSize(nodeBuf).x + sep + iconSz + iconTextGap + ImGui::CalcTextSize(edgeBuf).x + fpsSectionW + ImGui::GetStyle().WindowPadding.x;

        const float centredY = (barH - iconSz) * 0.5f;

        {
            ImVec2 wp = ImGui::GetWindowPos();
            float sepX = wp.x + ImGui::GetWindowWidth() - totalW - 12.5f;
            float sepY0 = wp.y + (barH - frameH) * 0.5f;
            dl->AddLine(ImVec2(sepX, sepY0), ImVec2(sepX, sepY0 + frameH), ImGui::ColorConvertFloat4ToU32(ColorScheme::Separator), 1.0f);
        }

        // Spinner for async operations (drawn left of the separator)
        if (m_controlData.engine.asyncOpActive.load(std::memory_order_acquire)) {
            ImVec2 wp = ImGui::GetWindowPos();
            float sepX = wp.x + ImGui::GetWindowWidth() - totalW - 12.5f;
            float barCenterY = wp.y + barH * 0.5f;

            const float spinnerRadius = 7.0f;
            const float dotRadius = 1.8f;
            const int numDots = 8;
            const float pi = 3.14159265358979323846f;
            float time = (float)ImGui::GetTime();

            ImVec2 spinnerCenter(sepX - 8.0f - spinnerRadius, barCenterY);
            for (int i = 0; i < numDots; i++) {
                float angle = time * 3.0f - i * (2.0f * pi / numDots);
                float alpha = 1.0f - (float)i / (float)numDots * 0.6f;
                ImU32 col = ImGui::GetColorU32(ImVec4(ColorScheme::Primary.x, ColorScheme::Primary.y, ColorScheme::Primary.z, alpha));
                float dx = cosf(angle) * spinnerRadius;
                float dy = sinf(angle) * spinnerRadius;
                dl->AddCircleFilled(ImVec2(spinnerCenter.x + dx, spinnerCenter.y + dy), dotRadius, col);
            }

            const float maxTextW = 250.0f;
            std::string desc(m_controlData.engine.asyncOpDesc);
            ImVec2 textSz = ImGui::CalcTextSize(desc.c_str());
            if (textSz.x > maxTextW) {
                while (desc.size() > 3 && ImGui::CalcTextSize((desc + "...").c_str()).x > maxTextW)
                    desc.pop_back();
                desc += "...";
                textSz = ImGui::CalcTextSize(desc.c_str());
            }

            float textEndX = spinnerCenter.x - spinnerRadius - 8.0f;
            float textX = textEndX - textSz.x;
            float textY = barCenterY - textSz.y * 0.5f;
            dl->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ColorScheme::TextMuted), desc.c_str());
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - totalW);
        ImGui::SetCursorPosY(centredY);

        // DB icon — pressable button, red/green tint for connection status
        {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImVec2 bMax = ImVec2(p.x + iconSz, p.y + iconSz);
            bool hov = mousePos.x >= p.x && mousePos.x <= bMax.x && mousePos.y >= p.y && mousePos.y <= bMax.y;
            if (hov || s_dbOpen)
                dl->AddRectFilled(p, bMax, ImGui::ColorConvertFloat4ToU32(ColorScheme::SurfaceLight), 4.0f);
            dl->AddImage((ImTextureID)(uintptr_t)m_dbIconTexture, p, bMax, ImVec2(0, 0), ImVec2(1, 1), ImGui::ColorConvertFloat4ToU32(dbTint));
            ImGui::InvisibleButton("##dbBtn", ImVec2(iconSz, iconSz));
            if (ImGui::IsItemClicked())
                s_dbOpen = !s_dbOpen;
            dbBtnHovered = ImGui::IsItemHovered();
            if (dbBtnHovered) {
                s_dbOpen = true;
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
            dbPanelPos = ImVec2(p.x + iconSz, barBottom);
        }

        ImGui::SameLine(0, sep);
        ImGui::SetCursorPosY(centredY);

        // Node (page) icon + count
        ImGui::ImageWithBg((ImTextureID)(uintptr_t)m_nodeIconTexture, ImVec2(iconSz, iconSz), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), iconTint);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Page Count");
        ImGui::SameLine(0, iconTextGap);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextMuted);
        ImGui::Text("%s", nodeBuf);
        ImGui::PopStyleColor();

        ImGui::SameLine(0, sep);
        ImGui::SetCursorPosY(centredY);

        // Edge (link) icon + count
        ImGui::ImageWithBg((ImTextureID)(uintptr_t)m_edgeIconTexture, ImVec2(iconSz, iconSz), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), iconTint);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Link Count");
        ImGui::SameLine(0, iconTextGap);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextMuted);
        ImGui::Text("%s", edgeBuf);
        ImGui::PopStyleColor();

        if (m_settings.showFPS) {
            ImGui::SameLine(0, sep);
            VSep();

            {
                ImVec2 p = ImGui::GetCursorScreenPos();
                float textH = ImGui::GetTextLineHeight();
                float barCenterY = ImGui::GetWindowPos().y + barH * 0.5f;
                ImU32 textCol = ImGui::ColorConvertFloat4ToU32(ColorScheme::TextMuted);
                float textRightX = p.x + fpsGroupW;

                const float iOff = (textH - fpsIconSz) * 0.5f;
                float groupH = textH * 2.0f + 2.0f;
                float lineY0 = barCenterY - groupH * 0.5f;
                float lineY1 = lineY0 + textH + 2.0f;

                // Eye icon + render fps
                dl->AddImage((ImTextureID)(uintptr_t)m_eyeIconTexture, ImVec2(p.x, lineY0 + iOff), ImVec2(p.x + fpsIconSz, lineY0 + iOff + fpsIconSz), ImVec2(0, 0), ImVec2(1, 1), textCol);
                dl->AddText(ImVec2(textRightX - ImGui::CalcTextSize(fpsBuf).x, lineY0), textCol, fpsBuf);

                // Atom icon + sim fps
                dl->AddImage((ImTextureID)(uintptr_t)m_atomIconTexture, ImVec2(p.x, lineY1 + iOff), ImVec2(p.x + fpsIconSz, lineY1 + iOff + fpsIconSz), ImVec2(0, 0), ImVec2(1, 1), textCol);
                dl->AddText(ImVec2(textRightX - ImGui::CalcTextSize(simBuf).x, lineY1), textCol, simBuf);

                if (ImGui::IsMouseHoveringRect(ImVec2(p.x, lineY0), ImVec2(textRightX, lineY0 + textH)))
                    ImGui::SetTooltip("FPS");
                if (ImGui::IsMouseHoveringRect(ImVec2(p.x, lineY1), ImVec2(textRightX, lineY1 + textH)))
                    ImGui::SetTooltip("Simulation FPS");

                ImGui::Dummy(ImVec2(fpsGroupW, iconSz));
            }
        }
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
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing);

        ImGui::PushItemWidth(panelSliderW);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);

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
        bool physicsWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        if (!physicsTabHovered && !physicsWindowHovered) {
            s_physicsCloseTimer += ImGui::GetIO().DeltaTime;
            if (s_physicsCloseTimer >= 0.15f) {
                s_physicsOpen = false;
                s_physicsCloseTimer = 0.0f;
            }
        } else {
            s_physicsCloseTimer = 0.0f;
        }
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
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing);

        ImGui::PushItemWidth(panelSliderW);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);

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
        ImGui::SliderFloat("Mouse Sensitivity", &m_controlData.engine.mouseSensitivity, 0.1f, 10.0f, "%.1f");
        ImGui::SliderFloat("Field of View", &m_controlData.engine.cameraFov, 30.0f, 120.0f, "%.1f°");

        ImGui::Spacing();
        ImGui::Text("Nodes");
        ImGui::Separator();
        ImGui::SliderFloat("Node Scale", &m_controlData.engine.nodeSizeMultiplier, 0.1f, 20.0f, "%.2f");
        ImGui::SliderInt("Search Result Limit", &m_controlData.engine.searchResultLimit, 0, 2000, m_controlData.engine.searchResultLimit == 0 ? "unlimited" : "%d");

        ImGui::Spacing();
        ImGui::Text("Labels");
        ImGui::Separator();
        ImGui::SliderFloat("Label Scale", &m_controlData.engine.labelSizeMultiplier, 0.1f, 10.0f, "%.2f");
        ImGui::SliderFloat("Label Distance", &m_controlData.engine.labelDistanceThreshold, 10.0f, 500.0f, "%.0f");
        ImGui::SliderInt("Max Labels", &m_controlData.engine.maxLabelCount, 10, 1000);

        ImGui::Spacing();
        ImGui::Text("Options");
        ImGui::Separator();
        ImGui::Checkbox("V-Sync", &m_controlData.engine.vSync);
        ImGui::Checkbox("Show FPS", &m_settings.showFPS);
        ImGui::Checkbox("Size Nodes by Link Count", &m_controlData.engine.sizeByDegree);

        ImGui::PopStyleColor();
        ImGui::PopItemWidth();
        bool renderingWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        if (!renderingTabHovered && !renderingWindowHovered) {
            s_renderingCloseTimer += ImGui::GetIO().DeltaTime;
            if (s_renderingCloseTimer >= 0.15f) {
                s_renderingOpen = false;
                s_renderingCloseTimer = 0.0f;
            }
        } else {
            s_renderingCloseTimer = 0.0f;
        }
    }

    // ── DB panel (dropdown below DB icon) ────────────────────────────────────
    if (s_dbOpen) {
        const float dbPanelW = 380.0f;
        const float dbPanelPad = 16.0f;
        ImGui::SetNextWindowViewport(vp->ID);
        ImGui::SetNextWindowPos(ImVec2(dbPanelPos.x - dbPanelW, dbPanelPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(ImVec2(dbPanelW, 0.0f), ImVec2(dbPanelW, FLT_MAX));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(dbPanelPad, 12.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Background);
        ImGui::PushStyleColor(ImGuiCol_Border, ColorScheme::Border);
        ImGui::Begin("##db_panel", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing);

        ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextSecondary);
        ImGui::PushItemWidth(dbPanelW - dbPanelPad * 2.0f);

        std::lock_guard<std::mutex> lock(m_controlData.app.dataSourceMutex);
        bool connected = m_controlData.app.dataSource.connectedToDataSource;

        int currentSourceType = static_cast<int>(m_controlData.app.dataSource.sourceType);
        const char *sourceTypes[] = {"HTTP Server", "Database (Neo4j)"};
        ImGui::Text("Data Source");
        if (ImGui::Combo("##sourceType", &currentSourceType, sourceTypes, IM_ARRAYSIZE(sourceTypes)))
            m_controlData.app.dataSource.sourceType = static_cast<dbInterfaceType>(currentSourceType);

        ImGui::Spacing();

        if (m_controlData.app.dataSource.sourceType == dbInterfaceType::DB) {
            ImGui::Text("URL");
            {
                char buffer[256];
                strncpy(buffer, m_controlData.app.dataSource.dbUrl.c_str(), sizeof(buffer));
                buffer[sizeof(buffer) - 1] = '\0';
                if (ImGui::InputTextWithHint("##neo4jUrl", "bolt://localhost", buffer, IM_ARRAYSIZE(buffer)))
                    m_controlData.app.dataSource.dbUrl = buffer;
            }
            ImGui::Text("Password");
            {
                char buffer[256];
                strncpy(buffer, m_controlData.app.dataSource.dbPassword.c_str(), sizeof(buffer));
                buffer[sizeof(buffer) - 1] = '\0';
                if (ImGui::InputTextWithHint("##neo4jPassword", "Enter password", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_Password))
                    m_controlData.app.dataSource.dbPassword = buffer;
            }
        } else if (m_controlData.app.dataSource.sourceType == dbInterfaceType::HTTP) {
            ImGui::Text("URL");
            {
                char buffer[256];
                strncpy(buffer, m_controlData.app.dataSource.serverUrl.c_str(), sizeof(buffer));
                buffer[sizeof(buffer) - 1] = '\0';
                if (ImGui::InputTextWithHint("##httpUrl", "http://eagombar.uk", buffer, IM_ARRAYSIZE(buffer)))
                    m_controlData.app.dataSource.serverUrl = buffer;
            }
        }

        ImGui::Spacing();

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::Primary);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::PrimaryHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::PrimaryActive);
        if (ImGui::Button("Save & Connect", ImVec2(-FLT_MIN, 0)))
            m_controlData.app.dataSource.attemptDataConnection = true;
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        ImGui::Spacing();

        ImDrawList *dbDl = ImGui::GetWindowDrawList();
        ImVec2 circlePos = ImGui::GetCursorScreenPos();
        circlePos.x += 8;
        circlePos.y += ImGui::GetTextLineHeight() * 0.5f;
        ImVec4 statusColor = connected ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
        dbDl->AddCircleFilled(circlePos, 6.0f, ImGui::ColorConvertFloat4ToU32(statusColor));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
        ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
        ImGui::Text("%s", connected ? "Connected" : "Disconnected");
        ImGui::PopStyleColor();

        ImGui::PopItemWidth();
        ImGui::PopStyleColor();
        bool dbWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenBlockedByPopup);
        bool dbPopupOpen = ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        if (!dbBtnHovered && !dbWindowHovered && !dbPopupOpen) {
            s_dbCloseTimer += ImGui::GetIO().DeltaTime;
            if (s_dbCloseTimer >= 0.15f) {
                s_dbOpen = false;
                s_dbCloseTimer = 0.0f;
            }
        } else {
            s_dbCloseTimer = 0.0f;
        }
    }

    // ── Suggestions (floating window below search input) ──────────────────────
    if (suggestionsVisible) {
        ImGui::SetNextWindowViewport(vp->ID);
        ImGui::SetNextWindowPos(ImVec2(searchScreenPos.x, searchScreenPos.y + frameH + 2.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(searchWidth, 0.0f), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorScheme::Surface);
        ImGui::PushStyleColor(ImGuiCol_Border, ColorScheme::Border);
        ImGui::Begin("##suggestions", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking);

        std::pair<std::string, std::string> selectedSuggestion;
        bool lockOk = false;
        int renderCount = 0;

        if (m_controlData.app.searchSuggestionsMutex.try_lock()) {
            lockOk = true;
            s_cachedSuggestions.clear();
            int count = 0;
            for (size_t i = 0; i < m_controlData.app.searchSuggestions.size(); i++) {
                if (m_controlData.app.searchSuggestions[i].first.empty())
                    continue;
                s_cachedSuggestions.push_back(m_controlData.app.searchSuggestions[i]);

                if (s_highlightedIdx == count)
                    ImGui::SetScrollHereY();

                bool isHighlighted = (s_highlightedIdx == count);
                if (isHighlighted)
                    ImGui::PushStyleColor(ImGuiCol_Header, ColorScheme::Primary);

                if (ImGui::Selectable(m_controlData.app.searchSuggestions[i].second.c_str(), isHighlighted)) {
                    selectedSuggestion = m_controlData.app.searchSuggestions[i];
                }

                if (isHighlighted)
                    ImGui::PopStyleColor();

                if (ImGui::IsItemHovered())
                    s_highlightedIdx = count;

                count++;
            }
            renderCount = count;
            
            if (renderCount == 0 && !s_cachedUserQuery.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                if (m_controlData.app.searchSuggestionsLoading.load(std::memory_order_acquire)) {
                    ImGui::Selectable("Loading...", false, ImGuiSelectableFlags_Disabled);
                } else {
                    ImGui::Selectable("No results found", false, ImGuiSelectableFlags_Disabled);
                }
                ImGui::PopStyleColor();
            }
            
            m_controlData.app.searchSuggestionsMutex.unlock();

            if (!selectedSuggestion.first.empty()) {
                s_pendingSetQuery = selectedSuggestion.second;
                {
                    std::lock_guard<std::mutex> lock(m_controlData.graph.searchStringMutex);
                    m_controlData.graph.searchString = selectedSuggestion.first;
                }
                m_controlData.graph.searching.store(true);
                suggestionsVisible = false;
                s_highlightedIdx = -1;
            }
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowHovered() &&
            !ImGui::IsMouseHoveringRect(searchScreenPos, ImVec2(searchScreenPos.x + searchWidth, searchScreenPos.y + frameH), false)) {
            suggestionsVisible = false;
            s_highlightedIdx = -1;
        }

        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }

    // Keyboard navigation for suggestions (runs every frame that suggestions
    // are visible, using cached data — not gated on the mutex lock above)
    if (suggestionsVisible) {
        int total = (int)s_cachedSuggestions.size();
        if (total > 0) {
            if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
                s_highlightedIdx = (s_highlightedIdx + 1) % total;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
                s_highlightedIdx = (s_highlightedIdx - 1 + total) % total;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_Tab) && s_highlightedIdx >= 0 && s_highlightedIdx < total) {
                const auto &chosen = s_cachedSuggestions[s_highlightedIdx];
                s_pendingSetQuery = chosen.second;
                {
                    std::lock_guard<std::mutex> lock(m_controlData.graph.searchStringMutex);
                    m_controlData.graph.searchString = chosen.first;
                }
                m_controlData.graph.searching.store(true);
                suggestionsVisible = false;
                s_highlightedIdx = -1;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            suggestionsVisible = false;
            s_highlightedIdx = -1;
        }
    }

    return interacted;
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
