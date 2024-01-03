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
  colorMin(new float[4]{0.462f, 0.7f, 0.917f, 1.0f}),
  colorMax(new float[4]{1.0f, 0.0f, 0.0f, 1.0f}),
  clipMin(0.0f),
  clipMax(4.0f),
  b(b){
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }
  // 16x antialiasing
  glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1200, 800, "SWE", NULL, NULL);
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

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
                                                        // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  setupShaders();
  glUseProgram(programID);

  // Set up vertex data and buffer(s) and attribute pointers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * b.getSize(), b.getData(), GL_STATIC_DRAW);
  // important that this comes after glBufferData
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);

  // setup width and height uniforms
  GLint widthLoc  = glGetUniformLocation(programID, "width");
  GLint heightLoc = glGetUniformLocation(programID, "height");
  std::cout << "widthLoc: " << b.getCols() << " heightLoc: " << b.getRows() << std::endl;
  glUniform1f(widthLoc, static_cast<GLfloat>(b.getCols()));
  glUniform1f(heightLoc, static_cast<GLfloat>(b.getRows()));


  // setup color uniforms
  minColorLoc = glGetUniformLocation(programID, "minColor");
  maxColorLoc = glGetUniformLocation(programID, "maxColor");

  // setup clip max uniform
  clipMaxLoc = glGetUniformLocation(programID, "clipMax");
  clipMinLoc = glGetUniformLocation(programID, "clipMin");

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

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);

  glfwTerminate();
}
void Gui::Gui::update(const Tools::Float2D<RealType>& h){

  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(programID);

  // Poll for and process events
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();


  unsigned long dataSize = b.getSize();

  auto data = std::vector<GLfloat>(b.getSize());
  // copy data from 2d h to 1d data
  for (int i = 0; i < b.getRows(); ++i) {
    for (int j = 0; j < b.getCols(); ++j) {
      data[i * h.getRows() + j] = h[j][i] + b[j][i];
    }
  }


  ImGui::Begin("Hello, world!");
  ImGui::ColorEdit4("Color for Min value", colorMin, ImGuiColorEditFlags_NoInputs);
  ImGui::ColorEdit4("Color for Max value", colorMax, ImGuiColorEditFlags_NoInputs);
  ImGui::DragFloat("Min Value", &clipMin, 0.005f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_None);
  ImGui::DragFloat("Max Value", &clipMax, 0.005f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_None);
  ImGui::End();

  glUniform4f(minColorLoc, colorMin[0], colorMin[1], colorMin[2], colorMin[3]);
  glUniform4f(maxColorLoc, colorMax[0], colorMax[1], colorMax[2], colorMax[3]);

  glUniform1f(clipMaxLoc, clipMax);
  glUniform1f(clipMinLoc, clipMin);

  // Update vertex buffer object with new data
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * dataSize, data.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);


  // Draw the data
  glPointSize(1.0f);
  glDrawArrays(GL_POINTS, 0, (int)dataSize);
  glBindVertexArray(0);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Swap the front and back buffers
  glfwSwapBuffers(window);


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

uniform vec4 minColor; // Color for the minimum value
uniform vec4 maxColor; // Color for the maximum value

uniform float clipMax; // Clip value at max
uniform float clipMin; // Clip value at min


void main() {
  // Clamp the value between the min and max values
  float color = clamp(VertexValue, clipMin, clipMax);
  // Map the value to the range 0.0 - 1.0
  color = (color - clipMin) / (clipMax - clipMin);
  // Linearly interpolate between the two colors
  FragColor = mix(minColor, maxColor, color);
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
