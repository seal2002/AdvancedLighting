#include <MainWindow.hpp>
#include <glm\glm.hpp>
#include <glm\common.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GLFW\include\GLFW\glfw3.h>
#include <glad\glad.h>

#include "common\Shader.h"
#include "common\Camera.h"
#include "common\LoadTexture.h"
#include "common\TextRendering.h"
#include "common\Object.h"

#define PATH "..\\Projects\\3.3.ShadowMapping"

using namespace OpenGLWindow;
void renderScene(Shader &shader);
void renderQuad();

void do_movement();

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool* keysPressed;
bool gamma = false;

static int SCR_W = 1280;
static int SCR_H = 720;

const int SHADOW_W = 1024;
const int SHADOW_H = 1024;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Object *cube;
Object *ground;

int main()
{
    Window window(SCR_W, SCR_H, "Shadow Mapping");

    glEnable(GL_DEPTH_TEST);

    string path(PATH);
    path += "\\ShadowMappingDepth";
    Shader simpleDepthshader(path);

    path = string(PATH);
    path += "\\ShadowMapping";
    Shader shader(path);

    path = string(PATH);
    path += "\\light";
    Shader shaderLight(path);

    path = string(PATH);
    path += "\\text";
    Shader shaderText(path);

    // Load Cube Object
    cube = new Object();
    cube->Load("..\\Resources\\Cube.txt");

    // Load Ground Object
    ground = new Object();
    ground->Load("..\\Resources\\ShadowMappingGround.txt");

    // load textures
    // -------------
    unsigned int woodTexture = loadTexture("..\\Resources\\wood.png");

    // configure depth map FBO

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // Create depth texture
    unsigned int depthMapTexture;
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_W, SHADOW_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    // -------------
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    // Apply shader
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 1);

    // Init Text FPS
    char textFPS[50];
    TextRendering textRendering(SCR_W, SCR_H);
    
    while (!window.shouldClose())
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float FPS = 1.0f / deltaTime;

        keysPressed = window.getKeyPress();
        do_movement();
        window.processInput();

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // 1. render depth of scene to texture (from light's perspective)
        //
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from ligh's point of view
        simpleDepthshader.use();
        simpleDepthshader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_W, SHADOW_H);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodTexture);
            glCullFace(GL_FRONT);
            renderScene(simpleDepthshader);
            glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_W, SCR_H);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. render scene as normal using the generated depth/shadow map
        // --------------------------------------------------------------
        glViewport(0, 0, SCR_W, SCR_H);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(45.0f, (float)SCR_W / (float)SCR_H, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        shader.setVec3("viewPos", camera.cameraPos);
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        renderScene(shader);

        // Render light
        shaderLight.use();
        shaderLight.setMat4("projection", projection);
        shaderLight.setMat4("view", view);
        glm::mat4 model;
        model = glm::mat4();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        cube->Render(shaderLight, model);

        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        sprintf(textFPS, "FPS = %f", FPS);
        textRendering.RenderText(shaderText, textFPS, 25.0f, SCR_H - 25.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        window.swapBuffers();
        window.pollEvents();
    }
}

void renderScene(Shader &shader)
{
    glm::mat4 model;
    // ground
    ground->Render(shader, model);

    // cubes
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    cube->Render(shader, model);
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    cube->Render(shader, model);
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    cube->Render(shader, model);
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