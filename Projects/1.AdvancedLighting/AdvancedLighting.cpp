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

#define PATH "..\\Projects\\1.AdvancedLighting"

using namespace OpenGLWindow;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool* keysPressed;
bool* keysReleased;
static bool Blinn = false;

static int SCR_W = 800;
static int SCR_H = 600;
Object *Ground;

int main()
{
    Window window(SCR_W, SCR_H, "Advanced Lighting - Blinn Phong Shader");

    glEnable(GL_DEPTH_TEST);

    string path(PATH);
    path += "\\BlinnPhongShading";
    Shader PhongShader(path.c_str());

    Ground = new Object();
    Ground->Load("..\\Resources\\Ground.txt");

    unsigned int groundTexture = loadTexture("..\\Resources\\wood.png");

    // lighting info
    // -------------
    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

    while (!window.shouldClose())
    {
        keysPressed = window.getKeyPress();
        keysReleased = window.getKeyRelease();
        if (keysReleased[GLFW_KEY_C])
        {
            Blinn = !Blinn;
            cout << (Blinn ? "Blinn" : "Phong") << endl;
        }
        window.processInput();
        window.movement(camera);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        PhongShader.use();
        glm::mat4 projection = glm::perspective(45.0f, (float) SCR_W / (float) SCR_H, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        PhongShader.setMat4("projection", projection);
        PhongShader.setMat4("view", view);
        // set light uniforms
        PhongShader.setVec3("viewPos", camera.cameraPos);
        PhongShader.setVec3("lightPos", lightPos);
        PhongShader.setInt("blinn", Blinn ? 1 : 0);

        glm::mat4 model;
        // floor
        Ground->Render(PhongShader, model);

        window.swapBuffers();
        window.pollEvents();
    }
}
