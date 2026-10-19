#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <cstddef>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Pos {
    float x, y, z;
};

Pos camera = {0, 0, -3};

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

    // Add a callback to handle window resizes to always match our viewport to
    // our window size
    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    return window;
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    };
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.z += 0.1;
    };
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.z -= 0.1;
    };
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.x += 0.1;
    };
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.x -= 0.1;
    };
}

int get_program(const char *vshader_src, const char *fshader_src) {
    // Setting up our graphics pipeline
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
        std::cout << "Compiled 1 shader." << std::endl;
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);
    return VAO;
}

std::string read_file(const std::string &path) {
    std::ifstream shader_src(path);
    std::stringstream stringstream;
    stringstream << shader_src.rdbuf();
    return stringstream.str();
}

int main() {
    // Setup Opengl
    GLFWwindow *window = setup_window(800, 600);
    if (window == nullptr) {
        return -1;
    }

    // clang-format off
    float vertices[] = {
       // pos              //texture coords
        -0.5 , -0.5, -0.5,  0, 0,    // 0 BTM LEFT 
        0.5  , -0.5, -0.5,  1, 0,    // 1 BTM RIGHT
        0.5  , 0.5,  -0.5,  1, 1,    // 2 TOP RIGHT
        -0.5 , 0.5,  -0.5,  0, 1,     // 3 TOP LEFT
        
        -0.5 , -0.5,  0.5,  0, 0,    // 0 BTM LEFT 
        0.5  , -0.5,  0.5,  1, 0,    // 1 BTM RIGHT
        0.5  , 0.5,   0.5,  1, 1,    // 2 TOP RIGHT
        -0.5 , 0.5,   0.5,  0, 1     // 3 TOP LEFT
    };

    unsigned int idx[] = {
        // Front
        0, 3, 2,
        0, 2, 1,

        // Back
        7, 6, 4,
        5, 6, 4,
 
        // Left
        0, 3, 7,
        0, 7, 4,
        
        // Right
        1, 2, 6,
        1, 6, 5,
        
        // Top
        3, 7, 6,
        3, 6, 2,
        
        // Bottom
        0, 4, 1,
        1, 4, 5,
    };
    // clang-format on

    // Setup the VAOs
    unsigned int vao = get_vao(vertices, idx, sizeof(vertices), sizeof(idx));
    glBindVertexArray(vao);

    // Load program
    unsigned int p1 = get_program(read_file("shaders/vertex.glsl").c_str(),
                                  read_file("shaders/fragment.glsl").c_str());
    glUseProgram(p1);

    // Instantiate texture
    int width, height, nrChannels;
    unsigned char *data =
        stbi_load("assets/meturben", &width, &height, &nrChannels, 0);
    unsigned int textid;
    glGenTextures(1, &textid);
    glBindTexture(GL_TEXTURE_2D, textid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Setup transformation matrix

    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    unsigned int world_loc = glGetUniformLocation(p1, "world");
    unsigned int view_loc = glGetUniformLocation(p1, "view");
    unsigned int projection_loc = glGetUniformLocation(p1, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    // Setup debugging logic
    const char *DEBUG = std::getenv("DEBUG");
    if (DEBUG != nullptr)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Generate Cubes
    Pos cubes[10];
    for (int i = 0; i < 10; i++) {
        cubes[i] = {(float)(rand() % 10), (float)(rand() % 10),
                    (float)(rand() % 10)};
    }

    // RENDER LOOP!!
    while (!glfwWindowShouldClose(window)) {
        // Process user input
        process_input(window);
        glm::mat4 view = glm::mat4(1.0f);
        view = translate(view, glm::vec3(camera.x, camera.y, camera.z));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

        // Clear info from last scene
        glClearColor(0.341, 0.337, 0.204, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rener cubes
        for (int i = 0; i < 10; i++) {
            glm::mat4 world = glm::mat4(1.0f);
            world = glm::translate(
                world, glm::vec3(cubes[i].x, cubes[i].y, cubes[i].z));
            world =
                glm::rotate(world, (float)glfwGetTime(), glm::vec3(1, 1, 0));
            glUniformMatrix4fv(world_loc, 1, GL_FALSE, glm::value_ptr(world));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // Not too sure exactly what events we are polling now?
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
