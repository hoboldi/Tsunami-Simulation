#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Tools/Float2D.hpp"
#include "Tools/RealType.hpp"

namespace Gui {
  class Gui {
  public:
    explicit Gui(Tools::Float2D<RealType>& b);
    void update(const Tools::Float2D<RealType>& h);
    ~Gui();

  private:
    GLFWwindow* window;
    GLuint programID;
    GLuint VBO, VAO;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint minColorLoc, maxColorLoc;
    float* colorMin, *colorMax;
    float clipMin, clipMax;
    GLint clipMaxLoc, clipMinLoc;
    Tools::Float2D<RealType>& b;
    void setupShaders();
  };

} // namespace Gui
