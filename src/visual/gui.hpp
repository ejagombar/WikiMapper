#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>
#include <string>

#include "../controlData.hpp"
#include "imgui.h"

struct GUISettings {
    bool debugMode = false;

    float searchTimeElapsed = 0.0f;
};

// A wrapper of Dear ImGui to provide a graphical interface for Wikimapper.
class GUI {
  public:
    GUI(GLFWwindow *m_window, std::string font, ControlData &controlData);
    ~GUI();

    void BeginFrame();
    void EndFrame();
    void EndFrameProper();
    void RenderMenu();
    void RenderDebugMenu();
    void AckReset();
    void SearchCompleted(std::string resultTitle);
    void RenderSearchBar();
    void RenderBottomLeftBox();
    void SetActiveNodeInfo(std::string activeNodeTitle);
    void SetOriginNodeInfo(std::string originNodeTitle);
    void setupTheme();

    bool Active();

    const GUISettings &GUIValues() { return m_settings; };

  private:
    void subtitle(const char *text);
    void separator();

    std::string m_activeNodeTitle;
    std::string m_originNodeTitle;

    ControlData &m_controlData;
    GUISettings m_settings;
    ImFont *m_defaultFont;
    ImFont *m_titleFont;
    ImFont *m_subTitleFont;
    bool m_overrideActive = false;
};

#endif // LABEL_H
