#ifndef GUI_CLASS_H
#define GUI_CLASS_H

#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"../imgui/imgui.h"
#include"../imgui/backends/imgui_impl_glfw.h"
#include"../imgui/backends/imgui_impl_opengl3.h"

#include"../include/mesh.h"

class Gui {
    public:
    static void Initialize(GLFWwindow* window);

    static void Begin();

    static void End();

    static void CleanUp();

    static void Transform(std::vector<Mesh> &meshes, std::vector<int> &currentMeshes, int &selectedMeshType);

    static void Lighting(glm::vec4 &lightColor, glm::vec3 &lightPos, float &attenuationScale);

    static void Debug(const double &mouseX, const double &mouseY);
};

#endif //GUI_CLASS_H
