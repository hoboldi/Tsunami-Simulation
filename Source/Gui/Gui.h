#pragma once
#include <algorithm>
#include <complex>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <vector>

#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "Tools/Float2D.hpp"
#include "Tools/RealType.hpp"

namespace Gui {
  class Gui {
  public:
    /**
     * @brief Construct a new Gui object
     * @param b bathymetry to display
     */
    explicit Gui(const Tools::Float2D<RealType>& b, int width, int height);
    /**
     * @brief Update the display
     * @param h water height
     * @param time current time
     */
    void update(const Tools::Float2D<RealType>& h, double time);

    std::pair<std::pair<int, int>, std::pair<int, int>> getStartEnd(const Tools::Float2D<RealType>& h);
    ~Gui();

    void setBathymetry(Tools::Float2D<RealType>& b);


  private:
    GLFWwindow*               window;
    GLuint                    programID;
    GLuint                    VBO, VAO;
    GLuint                    vertexShader;
    GLuint                    fragmentShader;
    GLint                     minColorLoc, maxColorLoc;
    float *                   colorMin, *colorMax;
    float                     clipMin, clipMax;
    GLint                     clipMaxLoc, clipMinLoc;
    Tools::Float2D<RealType> b_;
    GLfloat*                  data;
    void                      setupShaders();
  };

} // namespace Gui
