#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>
#include <string>

#include "imgui.h"

struct GUISettings {
    bool debugMode = true;
    bool wireFrame = false;
    bool vSync = true;
    float cameraFov = 45;
    float mouseSensitivity = 2.f;
    bool resetSimulation = false;

    bool searching = false;
    std::string searchString = "";
    float searchTimeElapsed = 0.0f;

    float qqMultiplier = 0.05f;
    float gravityMultiplier = 30.f;
    float accelSizeMultiplier = 0.01f;
    float targetDistance = 100;
};

// A wrapper of Dear ImGui to provide a graphical interface for Wikimapper.
class GUI {
  public:
    GUI(GLFWwindow *m_window, std::string font);
    ~GUI();

    void BeginFrame();
    void EndFrame();
    void RenderMenu();
    void RenderDebugMenu();
    void AckReset();
    void SearchCompleted(std::string resultTitle);
    void RenderSearchBar();
    void RenderBottomLeftBox();

    bool Active();

    const GUISettings &GUIValues() { return m_settings; };

  private:
    void subtitle(const char *text);
    void separator();

    GUISettings m_settings;
    ImFont *m_defaultFont;
    ImFont *m_titleFont;
    ImFont *m_subTitleFont;
};

#endif // LABEL_H
