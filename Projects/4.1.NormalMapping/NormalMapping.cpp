#include "MainWindow.hpp"

#include <glm\glm.hpp>
#include <glm\common.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GLFW\include\GLFW\glfw3.h>
#include <glad\glad.h>

#include "common\Camera.h"
#include "common\Shader.h"
#include "common\LoadTexture.h"
#include "common\Object.h"

using namespace OpenGLWindow;

#define PATH "..\\Projects\\4.1.NormalMapping"

const int SCR_W = 1280;
const int SCR_H = 720;
void do_movement();
bool* keysPressed;

Object cube;
Object face;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void main()
{
    Window window(SCR_W, SCR_H, "Normal Mapping");
    glEnable(GL_DEPTH_TEST);

    string path(PATH);
    path += "\\NormalMapping";
    Shader NormalShader(path);

    path = string(PATH);
    path += "\\light";
    Shader lightShader(path);

    path = string(PATH);
    path += "\\visualizingNormal";
    Shader visualizingNormalShader((path + ".vs").c_str(), (path + ".fs").c_str(), (path + ".gs").c_str());
    // load textures
    // -------------
    unsigned int wallTexture = loadTexture("..\\Resources\\brickwall.jpg");
    unsigned int normalTexture = loadTexture("..\\Resources\\brickwall_normal.jpg");

    // lighting info
    // -------------
    glm::vec3 lightPos(0.0f, 0.0f, 4.0f);
    path = string(PATH);
    path += "\\Cube.txt";
    // Init Cube
    cube.Load(path.c_str());
    path = string(PATH);
    path += "\\Face.txt";
    face.Load(path.c_str());

    NormalShader.use();
    NormalShader.setInt("diffuseTexture", 0);
    NormalShader.setInt("normalTexture", 1);

    while(!window.shouldClose())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        keysPressed = window.getKeyPress();
        do_movement();
        window.processInput();

        NormalShader.use();
        glm::mat4 projection = glm::perspective(45.0f, float(SCR_W) / float(SCR_H), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        NormalShader.setMat4("projection", projection);
        NormalShader.setMat4("view", view);


        // set light uniforms
        NormalShader.setVec3("viewPos", camera.cameraPos);
        NormalShader.setVec3("lightPos", lightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        glm::mat4 model;
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0));
        face.Render(NormalShader, model);

        //// Set attribute for Visualizing Normal Vector
        //visualizingNormalShader.use();
        //visualizingNormalShader.setMat4("projection", projection);
        //visualizingNormalShader.setMat4("view", view);

        //// Render Normal Vector
        //face.Render(visualizingNormalShader, model);

        // Render light
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::mat4();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        cube.Render(lightShader, model);

        window.swapBuffers();
        window.pollEvents();
    }
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
}