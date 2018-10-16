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

#define PATH "..\\Projects\\6.1.HDR"

const int SCR_W = 1280;
const int SCR_H = 720;

Object cube;
Object face;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool hdr = true;
float exposure = 1.0f;

void main()
{
    Window window(SCR_W, SCR_H, "HDR");
    glEnable(GL_DEPTH_TEST);

    string path(PATH);
    path += "\\hdr";
    Shader hdrShader(path);

    path = string(PATH);
    path += "\\light";
    Shader lightShader(path);

    // load textures
    // -------------
    unsigned int woodTexture = loadTexture("..\\Resources\\wood.png", true);

    // configure floating point framebuffer
    // ------------------------------------

    // create floating point color buffer
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_W, SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // create depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_W, SCR_H);
    // create frame buffer object
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    // -------------
    // positions
    std::vector<glm::vec3> lightPositions;
    lightPositions.push_back(glm::vec3( 0.0f,  0.0f, 49.5f)); // back light
    lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
    lightPositions.push_back(glm::vec3( 0.0f, -1.8f, 4.0f));
    lightPositions.push_back(glm::vec3( 0.8f, -1.7f, 6.0f));
    // colors
    std::vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
    lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
    lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
    lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

    // Init Vertex data
    cube.Load("..\\Resources\\Cube.txt");
    face.Load("..\\Resources\\Face.txt");

    // shader configuration
    // --------------------
    lightShader.use();
    lightShader.setInt("diffuseTexture", 0);

    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);

    while(!window.shouldClose())
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        window.movement(camera);

        // 1. render scene into floating point framebuffer
        // -----------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), float(SCR_W) / float(SCR_H), 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            
            lightShader.use();
            lightShader.setMat4("projection", projection);
            lightShader.setMat4("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodTexture);
            // set lighting uniforms
            for(int i = 0; i < lightPositions.size(); i++) {
                lightShader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
                lightShader.setVec3("lights[" + std::to_string(i) + "].Color", lightPositions[i]);
            }
            lightShader.setVec3("viewPos", camera.cameraPos);
            // render tunnel
            glm::mat4 model = glm::mat4();
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
            model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
            lightShader.setMat4("model", model);
            lightShader.setInt("inverse_normals", true);
            cube.Render(lightShader, model);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        hdrShader.setInt("hdr", hdr);
        hdrShader.setFloat("exposure", exposure);
        model = glm::mat4();
        face.Render(hdrShader, model);

        std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;

        window.swapBuffers();
        window.pollEvents();
    }
}