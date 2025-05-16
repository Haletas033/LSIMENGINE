#ifndef GUI_CLASS_H
#define GUI_CLASS_H

#include"../imgui/imgui.h"
#include"../imgui/backends/imgui_impl_glfw.h"
#include"../imgui/backends/imgui_impl_opengl3.h"

class Gui {
    public:
    void Initialize(GLFWwindow* window);

    void Begin();

    void End();

    void CleanUp();
};

#endif //GUI_CLASS_H
