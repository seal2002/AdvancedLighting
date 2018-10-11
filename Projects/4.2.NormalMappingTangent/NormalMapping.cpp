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

    // Init Vertex data
    cube.Load("..\\Resources\\Cube.txt");
    face.Load("..\\Resources\\Face.txt");

    NormalShader.use();
    NormalShader.setInt("diffuseTexture", 0);
    NormalShader.setInt("normalTexture", 1);

    while(!window.shouldClose())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        window.movement(camera);

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
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        face.Render(NormalShader, model);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0));
        face.Render(NormalShader, model);

        //// Set attribute for Visualizing Normal Vector
        visualizingNormalShader.use();
        visualizingNormalShader.setMat4("projection", projection);
        visualizingNormalShader.setMat4("view", view);

        // Render Normal Vector
        face.Render(visualizingNormalShader, model);

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