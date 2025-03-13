#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class GUI {
  public:
    GUI(GLFWwindow *m_window, std::string font);
    ~GUI();

    void BeginFrame();
    void EndFrame();
    void RenderMenu();
    void RenderOverlay();

  private:
    ImFont *m_defaultFont;
    ImFont *m_titleFont;
};

#endif // LABEL_H
