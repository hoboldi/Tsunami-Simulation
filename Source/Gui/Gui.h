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
    void update(const Tools::Float2D<RealType>& h) const;
    ~Gui();

  private:
    GLFWwindow* window;
    GLuint programID;
    GLuint VBO, VAO;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint minLoc, maxLoc;
    Tools::Float2D<RealType>& b;
    void setupShaders();
  };

} // namespace Gui
