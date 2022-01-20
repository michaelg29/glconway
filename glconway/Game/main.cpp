#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "conway.h"

void printTable(char** table, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%s\n", table[i]);
    }

    printf("\n\n");
}

// rendering parameters
const char* title = "Conway's Game of Life";
unsigned int width = 1000;
unsigned int height = 400;
double generationFrequency = 0.5; // time in between generations

// initialize GLFW
void initGLFW(unsigned int versionMajor, unsigned int versionMinor) {
    // initialize context
    glfwInit();

    // pass in parameters
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // macos specific parameter
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void createWindow(GLFWwindow*& window,
    const char* title, unsigned int width, unsigned int height,
    GLFWframebuffersizefun framebufferSizeCallback) {
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

/*
    Shader methods
*/

std::string readFile(const char* filename) {
    std::ifstream file;
    std::stringstream buf;

    std::string ret;

    // open file
    file.open(filename);

    if (file.is_open()) {
        // read file
        buf << file.rdbuf();
        ret = buf.str();
        file.close();
    }
    else {
        std::cout << "Could not read " << filename << std::endl;
    }

    return ret;
}

int genShader(const char* filename, GLenum type) {
    std::string shaderSrc = readFile(filename);
    const GLchar* shader = shaderSrc.c_str();

    // build and compile
    int shaderObj = glCreateShader(type);
    glShaderSource(shaderObj, 1, &shader, NULL);
    glCompileShader(shaderObj);

    // check for errors
    int success;
    char infoLog[512];
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderObj, 512, NULL, infoLog);
        std::cout << "Error in shader compilation: " << infoLog << std::endl;
        return -1;
    }

    return shaderObj;
}

int genShaderProgram(const char* vertexShaderPath,
    const char* fragmentShaderPath,
    const char* geoShaderPath) {
    int shaderProgram = glCreateProgram();

    // compile shaders
    int vShader = genShader(vertexShaderPath, GL_VERTEX_SHADER);
    int fShader = genShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
    int gShader = genShader(geoShaderPath, GL_GEOMETRY_SHADER);

    if (vShader == -1 || fShader == -1 || gShader == -1) {
        return -1;
    }

    // link
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glAttachShader(shaderProgram, gShader);
    glLinkProgram(shaderProgram);

    // check for errors
    int success;
    char infoLog[512];
    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error in shader linking: " << infoLog << std::endl;
        return -1;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    glDeleteShader(gShader);

    return shaderProgram;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    // close key
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    std::cout << "Hello, world!\n" << std::endl;

#define X 30
#define Y 100

    /*
        INIT CONWAY
    */
    conway c;
    conway_init(&c, 1, X, Y);

    // GLIDER GUN (X = 30, Y = 100)
    const char* seed[X] = {
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                1                                                                   ",
        "                              1 1                                                                   ",
        "                    11      11            11                                                        ",
        "                   1   1    11            11                                                        ",
        "        11        1     1   11                                                                      ",
        "        11        1   1 11    1 1                                                                   ",
        "                  1     1       1                                                                   ",
        "                   1   1                                                                            ",
        "                    11                                                                              ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    ",
        "                                                                                                    " };
    conway_seedTable(&c, (char**)seed, ' ');

    int nr = 800;
    char** output = conway_print(&c, '0', ' ', NULL);
    printTable(output, c.x);
    //system("cls");

    /*
        INIT OPENGL
    */

    initGLFW(3, 3);

    // window
    GLFWwindow* window = nullptr;
    createWindow(window, title, width, height, framebufferSizeCallback);

    // load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }

    // set viewport
    framebufferSizeCallback(window, width, height);

    // timing variables
    double dt = 0.0;
    double lastFrame = 0.0;
    double timeSinceLastGen = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        // update time
        dt = glfwGetTime() - lastFrame;
        lastFrame += dt;

        processInput(window);

        // increment timer
        timeSinceLastGen += dt;

        if (timeSinceLastGen >= generationFrequency) {
            // new generation
            conway_simulate(&c);
            output = conway_print(&c, '0', ' ', output);
            printTable(output, c.x);

            // reset timer
            timeSinceLastGen = 0.0;
        }

        // get new input
        glfwPollEvents();
    }

    for (int x = 0; x < c.x; x++)
    {
        free(output[x]);
    }
    free(output);

    // clear conway
    conway_destroy(&c);

    // terminate GLFW
    glfwTerminate();

    return 0;
}