#include <MainWindow.hpp>
#include <glm\glm.hpp>
#include <glm\common.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GLFW\include\GLFW\glfw3.h>
#include <glad\glad.h>

#include "common\Shader.h"
#include "common\Camera.h"
#include "common\LoadTexture.h"
#include "common\Object.h"

#define PATH "..\\Projects\\2.GammaCorrection"

using namespace OpenGLWindow;

void do_movement();
unsigned int loadTexture(const char *path);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool* keysPressed;
bool* keysReleased;
static bool gamma = false;

static int SCR_W = 1280;
static int SCR_H = 720;
Object *Ground;

int main()
{
    Window window(SCR_W, SCR_H, "Gamma Correction");

    glEnable(GL_DEPTH_TEST);

    string path(PATH);
    path += "\\GammaCorrection";
    Shader shader(path.c_str());

    Ground = new Object();
    Ground->Load("..\\Resources\\Ground.txt");

    // load textures
    // -------------
    unsigned int floorTexture = loadTexture("..\\Resources\\wood.png");
    unsigned int floorTextureGammaCorrected = loadTexture("..\\Resources\\wood.png", true);

    // lighting info
    // -------------
    glm::vec3 lightPositions[] = {
        glm::vec3(-3.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(3.0f, 0.0f, 0.0f)
    };
    glm::vec3 lightColors[] = {
        glm::vec3(0.25),
        glm::vec3(0.50),
        glm::vec3(0.75),
        glm::vec3(1.00)
    };

    while (!window.shouldClose())
    {
        keysPressed = window.getKeyPress();
        keysReleased = window.getKeyRelease();
        do_movement();
        window.processInput();

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        shader.use();
        glm::mat4 projection = glm::perspective(45.0f, (float)SCR_W / (float)SCR_H, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        glUniform3fv(glGetUniformLocation(shader.Program, "lightPositions"), 4, &lightPositions[0][0]);
        glUniform3fv(glGetUniformLocation(shader.Program, "lightColors"), 4, &lightColors[0][0]);
        shader.setVec3("viewPos", camera.cameraPos);
        shader.setInt("gamma", gamma);
        glm::mat4 model;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gamma ? floorTextureGammaCorrected : floorTexture);
		// floor
		Ground->Render(shader, model);

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
    if (keysReleased[GLFW_KEY_C])
    {
        gamma = !gamma;
    }
}