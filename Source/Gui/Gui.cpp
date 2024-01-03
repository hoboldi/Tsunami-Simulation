#include "Gui.h"

#include <algorithm>
#include <complex>


Gui::Gui::Gui(Tools::Float2D<RealType>& b):
  window(nullptr),
  programID(0),
  VBO(0),
  VAO(0),
  vertexShader(0),
  fragmentShader(0),
  b(b) {
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }
  // 4x antialiasing
  glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1024, 768, "SWE", NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window); // Initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    exit(EXIT_FAILURE);
  }

  setupShaders();
  glUseProgram(programID);

  // Set up vertex data and buffer(s) and attribute pointers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * b.getSize(), b.getData(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);

  // setup width and height uniforms
  GLint widthLoc  = glGetUniformLocation(programID, "width");
  GLint heightLoc = glGetUniformLocation(programID, "height");
  std::cout << "widthLoc: " << b.getCols() << " heightLoc: " << b.getRows() << std::endl;
  glUniform1f(widthLoc, static_cast<GLfloat>(b.getCols()));
  glUniform1f(heightLoc, static_cast<GLfloat>(b.getRows()));

  // setup min and max uniforms (not used currently)
  minLoc = glGetUniformLocation(programID, "minValue");
  maxLoc = glGetUniformLocation(programID, "maxValue");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
Gui::Gui::~Gui() {
  // Cleanup resources (delete shaders, buffers, etc.)
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(programID);

  glDetachShader(programID, vertexShader);
  glDetachShader(programID, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glfwTerminate();
}
void Gui::Gui::update(const Tools::Float2D<RealType>& h) const {

  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(programID);

  unsigned long dataSize = b.getSize();

  auto data = std::vector<GLfloat>(b.getSize());
  // copy data from 2d h to 1d data
  for (int i = 0; i < b.getRows(); ++i) {
    for (int j = 0; j < b.getCols(); ++j) {
      data[i * h.getRows() + j] = h[j][i] + b[j][i];
    }
  }


//  // Find the min and max values in the data and apply log scaling
//  GLfloat minValue = *std::min_element(data.begin(), data.end());
//  GLfloat maxValue = *std::max_element(data.begin(), data.end());
//  std::cout << "min: " << minValue << " max: " << maxValue << std::endl;
//
//  glUniform1f(minLoc, minValue);
//  glUniform1f(maxLoc, maxValue);
  // Update vertex buffer object with new data
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * dataSize, data.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);


  // Draw the data
  glPointSize(0.5f);
  glDrawArrays(GL_POINTS, 0, (int)dataSize);
  glBindVertexArray(0);
  // Swap the front and back buffers
  glfwSwapBuffers(window);

  // Poll for and process events
  glfwPollEvents();


  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cout << "OpenGL error: " << error << std::endl;
  }
}
void Gui::Gui::setupShaders() {

  const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in float value;

uniform float width;  // Width of the 2D array
uniform float height; // Height of the 2D array

out float VertexValue;  // New varying variable

void main() {
    // Calculate x and y  coordinates
    float x = float(gl_VertexID % int(width)) / (width);
    float y = float(gl_VertexID / int(width)) / (height);

    // Map the coordinates to clip space
    gl_Position = vec4(2.0*x -1.0, 2.0 * y - 1.0, 0.0, 1.0);
    VertexValue = value; // Pass the value to the fragment shader
}
)";

  const char* fragmentShaderSource = R"(
#version 330 core

in float VertexValue;  // Receive the value from the vertex shader

out vec4 FragColor;

void main() {

  FragColor = vec4(1.0 - VertexValue ,0.0, VertexValue, 1.0);
}
)";

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  programID = glCreateProgram();
  glAttachShader(programID, vertexShader);
  glAttachShader(programID, fragmentShader);
  glLinkProgram(programID);
}
