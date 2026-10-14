#include <cstddef>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

GLFWwindow *setup_window(int default_width, int default_height) {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a window
  GLFWwindow *window = glfwCreateWindow(default_width, default_height,
                                        "Triangle baby", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create window" << std::endl;
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);

  // Pass GLAD glfw's proc getter...??? Not very sure what this is for
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return nullptr;
  }

  // Set the viewport. Right now i'm not too sure whats the difference between
  // window size and viewport size?
  glViewport(0, 0, default_width, default_height);

  // Add a callback to handle window resizes to always match our viewport to our
  // window size
  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *window, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });
  return window;
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  };
}

int main() {
  // Setup Opengl
  GLFWwindow *window = setup_window(800, 600);
  if (window == nullptr) {
    return -1;
  }

  // clang-format off
  float vertices[] = {
    -0.5,-0.5,0,
    0.5,-0.5,0,
    0,0.5,0,
  };
  // clang-format on

  // Setting up our graphics pipeline
  const char *vshader_src =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "void main() {\n"
      "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1);\n"
      "}\0";
  const char *fshader_src = "#version 330 core\n"
                            "out vec4 FragColor;\n"
                            "void main() {\n"
                            "    FragColor = vec4(0,1,0,1);\n"
                            "}\0";

  // Start a VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Get 1 VBO Buffer object
  unsigned int VBO;
  glGenBuffers(1, &VBO);

  // Set type for VBO object
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Create Shaders
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vertex_shader, 1, &vshader_src, NULL);
  glShaderSource(fragment_shader, 1, &fshader_src, NULL);
  glCompileShader(vertex_shader);
  glCompileShader(fragment_shader);

  // Check for shader compile errors
  int success;
  for (const auto &e : {vertex_shader, fragment_shader}) {
    glGetShaderiv(e, GL_COMPILE_STATUS, &success);
    if (!success) {
      std::cout << "Failed to compile shader" << std::endl;
      return -1;
    }
  }

  // String all the shaders together into a program
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  // Clean up
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // Use program
  glUseProgram(shader_program);

  // Tell graphics pipeline how to read data in VBO
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // RENDER LOOP!!
  while (!glfwWindowShouldClose(window)) {
    // Process user input
    process_input(window);

    // Render stuff
    glClearColor(1, 0, 0, 1);     // State setting
    glClear(GL_COLOR_BUFFER_BIT); // State using

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Not too sure exactly what events we are polling now?
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
