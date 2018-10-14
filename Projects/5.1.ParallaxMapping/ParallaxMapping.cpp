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

#define PATH "..\\Projects\\5.1.ParallaxMapping"

const int SCR_W = 1280;
const int SCR_H = 720;

Object cube;
Object face;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float heightScale = 0.1f;

void main()
{
    Window window(SCR_W, SCR_H, "Parallax Mapping");
    glEnable(GL_DEPTH_TEST);

    string path(PATH);
    path += "\\ParallaxMapping";
    Shader shader(path);

    path = string(PATH);
    path += "\\light";
    Shader lightShader(path);

    path = string(PATH);
    path += "\\visualizingNormal";
    Shader visualizingShader((path + ".vs").c_str(), (path + ".fs").c_str(), (path + ".gs").c_str());
    // load textures
    // -------------
    unsigned int wallTexture = loadTexture("..\\Resources\\brickwall2.jpg");
    unsigned int normalTexture = loadTexture("..\\Resources\\brickwall2_normal.jpg");
    unsigned int depthTexture = loadTexture("..\\Resources\\brickwall2_displacement.jpg");

    // lighting info
    // -------------
    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

    // Init Vertex data
    cube.Load("..\\Resources\\Cube.txt");
    face.Load("..\\Resources\\Face.txt", true);

    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("normalTexture", 1);
    shader.setInt("depthTexture", 2);

    while(!window.shouldClose())
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        window.movement(camera);

        shader.use();
        glm::mat4 projection = glm::perspective(camera.zoom, float(SCR_W) / float(SCR_H), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);


        // set light uniforms
        shader.setVec3("viewPos", camera.cameraPos);
        shader.setVec3("lightPos", lightPos);
        shader.setFloat("heightScale", heightScale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        glm::mat4 model;
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        face.Render(shader, model);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        face.Render(shader, model);

        //// Set attribute for Visualizing Normal Vector
        visualizingShader.use();
        visualizingShader.setMat4("projection", projection);
        visualizingShader.setMat4("view", view);

        // Render Normal Vector
        face.Render(visualizingShader, model);

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