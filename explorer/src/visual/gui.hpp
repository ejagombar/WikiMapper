#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>
#include <string>

#include "imgui.h"

class GUI {
  public:
    GUI(GLFWwindow *m_window, std::string font);
    ~GUI();

    void BeginFrame();
    void EndFrame();
    void RenderMenu();
    void RenderOverlay();

  private:
    void subtitle(const char *text);
    void separator();

    ImFont *m_defaultFont;
    ImFont *m_titleFont;
    ImFont *m_subTitleFont;
};

#endif // LABEL_H
