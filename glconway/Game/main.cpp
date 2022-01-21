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
unsigned int width = 100;
unsigned int height = 30;
unsigned int cellDim = 20;

double generationFrequency = 0.025; // time in between generations

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

void renderScreen(GLFWwindow* window, GLuint shaderProgram, conway c, GLuint VAO, GLuint VBO) {
    // clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update data
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, c.x * c.y, c.board);

    // render object
    glUseProgram(shaderProgram);
    glDrawArrays(GL_POINTS, 0, c.x * c.y);

    // swap buffers
    glfwSwapBuffers(window);
}

void terminate(conway* c, char **conwayOutput) {
    // clear conway
    conway_destroy(c);

    // terminate GLFW
    glfwTerminate();

    for (int x = 0; x < c->x; x++)
    {
        free(conwayOutput[x]);
    }
    free(conwayOutput);
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
    createWindow(window, title, width * cellDim, height * cellDim, framebufferSizeCallback);
    if (!window) {
        std::cout << "Could not create window" << std::endl;
        terminate(&c, output);
        return -1;
    }

    // load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Could not load GLAD" << std::endl;
        terminate(&c, output);
        return -1;
    }

    // set viewport
    framebufferSizeCallback(window, width * cellDim, height * cellDim);

    /*
        setup shaders
    */
    GLuint shaderProgram = genShaderProgram("main.vs", "main.fs", "main.gs");
    // set dimensions
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "width"), c.y);
    glUniform1f(glGetUniformLocation(shaderProgram, "cellWidth"), 2.0f / (float)width);
    glUniform1f(glGetUniformLocation(shaderProgram, "cellHeight"), 2.0f / (float)height);

    std::cout << 2.0f / (float)width << ' ' << 2.0f / (float)height << std::endl;

    /*
        setup VAO/VBO
    */
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set data
    glBufferData(GL_ARRAY_BUFFER, c.x * c.y, c.board, GL_DYNAMIC_DRAW);
    // set attribute pointers
    glEnableVertexAttribArray(0);
    // attribute index 0: 1 GL_BYTE per vertex, stride sizeof(char) to get to next vertex
    glVertexAttribIPointer(0, 1, GL_BYTE, sizeof(char), 0);

    // timing variables
    double dt = 0.0;
    double lastFrame = 0.0;
    double timeSinceLastGen = 0.0;

    // render initial configuration
    renderScreen(window, shaderProgram, c, VAO, VBO);

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

            // render update state
            renderScreen(window, shaderProgram, c, VAO, VBO);

            // reset timer
            timeSinceLastGen = 0.0;
        }

        // get new input
        glfwPollEvents();
    }

    // clear buffers/arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // delete buffers/arrays
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    // delete shaders
    glDeleteProgram(shaderProgram);

    std::cout << "Goodbye" << std::endl;
    terminate(&c, output);

    return 0;
}