#include <cstddef>
#include <cstdlib>
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

int get_program(int c) {
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
  // TODO: Hard code another shader for now
  const char *fshader_src2 = "#version 330 core\n"
                             "out vec4 FragColor;\n"
                             "void main() {\n"
                             "    FragColor = vec4(1,0,0,1);\n"
                             "}\0";
  // Create Shaders
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vertex_shader, 1, &vshader_src, NULL);
  glShaderSource(fragment_shader, 1, c ? &fshader_src : &fshader_src2, NULL);
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
  return shader_program;
}

int get_vao(float *vertices, unsigned int *indices, size_t v_len,
            size_t i_len) {
  // Create VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Create VBO for our vertices
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, v_len, vertices, GL_STATIC_DRAW);

  // Create EBO for the indices we actually want to draw
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_len, indices, GL_STATIC_DRAW);

  // Tell graphics pipeline how to read data in VBO
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Unbind VAO
  glBindVertexArray(0);
  return VAO;
}

int main() {
  // Setup Opengl
  GLFWwindow *window = setup_window(800, 600);
  if (window == nullptr) {
    return -1;
  }

  // clang-format off
  float vertices[] = {
    -0.5,-0.5,0,            // 0 
    0.5,-0.5,0,             // 1
    0.5,0.5,0,              // 2
    -0.5,0.5,0,           // 3
  };

  unsigned int idx1[] = {
    0, 3, 2,
  };
  unsigned int idx2[] = {
    0, 1, 2,
  };
  // clang-format on

  // Setup the VAOs
  unsigned int vao1 = get_vao(vertices, idx1, sizeof(vertices), sizeof(idx1));
  unsigned int vao2 = get_vao(vertices, idx2, sizeof(vertices), sizeof(idx2));

  // Load program
  unsigned int p1 = get_program(0);
  unsigned int p2 = get_program(1);

  const char *DEBUG = std::getenv("DEBUG");
  if (DEBUG != nullptr)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // RENDER LOOP!!
  while (!glfwWindowShouldClose(window)) {
    // Process user input
    process_input(window);

    // Render stuff
    glClearColor(0, 0, 0, 0);     // State setting
    glClear(GL_COLOR_BUFFER_BIT); // State using

    glBindVertexArray(vao1);
    glUseProgram(p1);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(vao2);
    glUseProgram(p2);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // Not too sure exactly what events we are polling now?
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
