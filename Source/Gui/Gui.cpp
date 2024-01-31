#include "Gui.h"


Gui::Gui::Gui(const Tools::Float2D<RealType>& b, int width, int height) :
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
  b_{b, false}
{
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }
  // 16x antialiasing
  glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // scale width and height so that the window fits on the screen
  while (width >= 1920 || height >= 1080) {
    width /= 1.2;
    height /= 1.2;
  }
  std::cout << "width: " << width << " height: " << height << std::endl;
  // Open a window and create its OpenGL context
  window = glfwCreateWindow(width, height, "SWE", NULL, NULL);
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
  unsigned long dataSize = b_.getSize() * sizeof(GLfloat);

  data = new GLfloat[dataSize];

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  setupShaders();
  glUseProgram(programID);

  // enable point size
  glEnable(GL_PROGRAM_POINT_SIZE);

  // Set up vertex data and buffer(s) and attribute pointers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * b.getSize(), nullptr, GL_STATIC_DRAW);
  // important that this comes after glBufferData
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);

  // setup width and height uniforms
  GLint widthLoc  = glGetUniformLocation(programID, "width");
  GLint heightLoc = glGetUniformLocation(programID, "height");
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
  delete[] data;
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
void Gui::Gui::update(const Tools::Float2D<RealType>& h, double time){

  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(programID);

  // Poll for and process events
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  unsigned long dataSize = b_.getSize() * sizeof(GLfloat);

  int x = b_.getCols();
  int y = b_.getRows();
  // copy data from 2d h to 1d data
  for (int i = 0; i < x; ++i) {
    for (int j = 0; j < y; ++j) {
      data[i* x + j] = static_cast<GLfloat>(h[j][i] + b_[j][i]);
    }
  }

  ImGui::Begin("Color Map");
  ImGui::ColorEdit4("Color for Min value", colorMin, ImGuiColorEditFlags_NoInputs);
  ImGui::ColorEdit4("Color for Max value", colorMax, ImGuiColorEditFlags_NoInputs);
  ImGui::DragFloat("Min Value", &clipMin, 0.005f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_None);
  ImGui::DragFloat("Max Value", &clipMax, 0.005f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_None);
  ImGui::End();

  // metadata display
  ImGui::Begin("Metadata");
  ImGui::Text("Time: %f", time);
  ImGui::Text("Width: %d", b_.getCols() -2);
  ImGui::Text("Height: %d", b_.getRows() -2);
  ImGui::End();

  glUniform4f(minColorLoc, colorMin[0], colorMin[1], colorMin[2], colorMin[3]);
  glUniform4f(maxColorLoc, colorMax[0], colorMax[1], colorMax[2], colorMax[3]);

  glUniform1f(clipMaxLoc, clipMax);
  glUniform1f(clipMinLoc, clipMin);

  // Update vertex buffer object with new data
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // calculate point size based on window size and data size
  // current window size
  std::pair<int, int> windowSize;
  glfwGetWindowSize(window, &windowSize.first, &windowSize.second);
  float pointSize = std::fmin(windowSize.first / (float)b_.getCols(), windowSize.second / (float)b_.getRows());

  // Draw the data
  glPointSize(pointSize);
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
  // if value is FLT_MAX, set color to black
  if (VertexValue == -3.4028234663852886E+38) {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }
  // if value is -FLT_MAX, set color to white
  if (VertexValue == 3.4028234663852886E+38) {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    return;
  }
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




std::pair<std::pair<int, int>, std::pair<int, int>> Gui::Gui::getStartEnd(const Tools::Float2D<RealType>& h) {
  std::pair<int, int> start{-1, -1};
  std::pair<int, int> end{-1, -1};
  bool startSet = false;
  bool endSet = false;


  unsigned long dataSize = h.getSize() * sizeof(GLfloat);
  std::cout << "dataSize: " << dataSize<< " " << b_.getCols() << " " << b_.getRows() << std::endl;
  std::cout << "h size: " << h.getSize() << " " << h.getCols() << " " << h.getRows() << std::endl;

  int x = b_.getCols();
  int y = b_.getRows();
  // copy data from 2d h to 1d data
  for (int i = 0; i < x; ++i) {
    for (int j = 0; j < y; ++j) {
      data[i* x + j] = h[j][i] + b_[j][i];
    }
  }

  glUseProgram(programID);

  glUniform4f(minColorLoc, colorMin[0], colorMin[1], colorMin[2], colorMin[3]);
  glUniform4f(maxColorLoc, colorMax[0], colorMax[1], colorMax[2], colorMax[3]);

  glUniform1f(clipMaxLoc, clipMax);
  glUniform1f(clipMinLoc, clipMin);



  bool continueLoop = true;
  while(continueLoop){
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programID);

    // Poll for and process events
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ImGui::Begin("Pick start and end points");
    ImGui::Text("Width: %d", b_.getCols() - 2);
    ImGui::Text("Height: %d", b_.getRows() - 2);

    // current window size
    std::pair<int, int> windowSize;
    glfwGetWindowSize(window, &windowSize.first, &windowSize.second);
    auto mousePosInWindow = ImGui::GetMousePos();
    // scale the cursor to the size of h
    mousePosInWindow.x /= windowSize.first;
    mousePosInWindow.y /= windowSize.second;
    // scale the cursor to the size of the window
    mousePosInWindow.x *= b_.getCols() - 2;
    mousePosInWindow.y *= b_.getRows() - 2;
    mousePosInWindow.y = (b_.getRows() - 2) - mousePosInWindow.y;
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      if(mousePosInWindow.x < 0 || mousePosInWindow.x > b_.getCols() - 2 || mousePosInWindow.y < 0 || mousePosInWindow.y > b_.getRows() - 2){
        continue;
      }
      if (!startSet && start.first == -1 && start.second == -1) {
        start    = {mousePosInWindow.x, mousePosInWindow.y};
        startSet = true;
      }
      else if (!endSet && end.first == -1 && end.second == -1) {
        end    = {mousePosInWindow.x, mousePosInWindow.y};
        endSet = true;
      }
      else {
        startSet = true;
        endSet   = true;
      }
    }

    ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePosInWindow.x, mousePosInWindow.y);
    ImGui::Text("Start: (%d, %d)", start.first, start.second);
    ImGui::Text("End: (%d, %d)", end.first, end.second);
    if (ImGui::Button("Reset")) {
      startSet = false;
      endSet = false;
      start = {-1, -1};
      end = {-1, -1};
    }
    if (ImGui::Button("Continue")) {
      continueLoop = false;
    }
    ImGui::End();
    // calculate point size based on window size and data size
    float pointSize = std::fmin(windowSize.first / (float)b_.getCols(), windowSize.second / (float)b_.getRows());

    // Draw the data
    glPointSize(pointSize);
    glDrawArrays(GL_POINTS, 0, (int)dataSize);
    glBindVertexArray(0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Swap the front and back buffers
    glfwSwapBuffers(window);
  }

  return std::make_pair(start, end);
}
