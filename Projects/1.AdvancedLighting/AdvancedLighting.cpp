#include <MainWindow.hpp>
#include <glm\glm.hpp>
#include <glm\common.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GLFW\include\GLFW\glfw3.h>
#include <glad\glad.h>

#include "common\Shader.h"
#include "common\Camera.h"
#include "common\LoadTexture.h"

#define PATH "..\\Projects\\1.AdvancedLighting"

using namespace OpenGLWindow;

void do_movement();
unsigned int loadTexture(const char *path);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool* keysPressed;
bool* keysReleased;
static bool Blinn = false;

static int SCR_W = 800;
static int SCR_H = 600;

float groundVertices[] = {
    // positions            // normals         // texcoords
    10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

    10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
    10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};

int main()
{
    Window window(SCR_W, SCR_H, "Advanced Lighting - Blinn Phong Shader");

    glEnable(GL_DEPTH_TEST);

    string path(PATH);
    path += "\\PhongShading";
    Shader PhongShader(path.c_str());

    unsigned int groundVAO, groundVBO;
    glGenBuffers(1, &groundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &groundVAO);
    glBindVertexArray(groundVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));

    unsigned int groundTexture = loadTexture("..\\Resources\\wood.png");
    
    PhongShader.Use();
    PhongShader.setInt("texture1", 0);
    
    // lighting info
    // -------------
    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
    
    
    while (!window.shouldClose())
    {
        keysPressed = window.getKeyPress();
        keysReleased = window.getKeyRelease();
        do_movement();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        PhongShader.Use();
        glm::mat4 projection = glm::perspective(45.0f, (float) SCR_W / (float) SCR_H, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        PhongShader.setMat4("projection", projection);
        PhongShader.setMat4("view", view);
        // set light uniforms
        PhongShader.setVec3("viewPos", camera.cameraPos);
        PhongShader.setVec3("lightPos", lightPos);
        PhongShader.setInt("blinn", Blinn ? 1 : 0);
        
        // floor
        glBindVertexArray(groundVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        window.swapBuffers();
        window.pollEvents();
    }
    glDeleteBuffers(1, &groundVBO);
    glDeleteVertexArrays(1, &groundVAO);
}

void do_movement()
{
    if (keysPressed[GLFW_KEY_W])
    {
        camera.ProcessKeyboard(Camera_Movement::FORWARD);
    }
    if (keysPressed[GLFW_KEY_S])
    {
        camera.ProcessKeyboard(Camera_Movement::BACKWARD);
    }
    if (keysPressed[GLFW_KEY_A])
    {
        camera.ProcessKeyboard(Camera_Movement::LEFT);
    }
    if (keysPressed[GLFW_KEY_D])
    {
        camera.ProcessKeyboard(Camera_Movement::RIGHT);
    }
    if (keysPressed[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(Camera_Movement::ROTATE_LEFT);
    }
    if (keysPressed[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(Camera_Movement::ROTATE_RIGHT);
    }
    if (keysPressed[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(Camera_Movement::ROTATE_UP);
    }
    if (keysPressed[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(Camera_Movement::ROTATE_DOWN);
    }
    if (keysReleased[GLFW_KEY_C])
    {
        Blinn = !Blinn;
    }
}