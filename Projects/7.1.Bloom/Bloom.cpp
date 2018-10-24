#include "MainWindow.hpp"

#include <glad/glad.h>

#include "common/Camera.h"
#include "common/Shader.h"
#include "common/LoadTexture.h"
#include "common/Object.h"

using namespace OpenGLWindow;

#define PATH "..\\Projects\\7.1.Bloom"

const int SCR_W = 1280;
const int SCR_H = 720;

void LoadWorld(vector<Object*> objects);

void main()
{
    Window window(SCR_W, SCR_H, "Bloom");
    
    // Load textures
    // -------------
    unsigned int woodTexture = loadTexture("..\\Resources\\wood.png");
    unsigned int containerTexture = loadTexture("..\\Resources\\container2.png");
    // Load shaders
    // ------------
    string path(PATH);
    path += "\\shader";
    Shader Shader(path);

    vector<Object*> objects;
}