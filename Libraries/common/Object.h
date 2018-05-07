#pragma once

#include <glm\glm.hpp>
#include <glm\common.hpp>
#include <glad\glad.h>

#include "Shader.h"

class Object
{
private:
    unsigned int VAO, VBO;
    float *verticle;
    int numline;
public:
    void Render(Shader &shader, glm::mat4 model)
    {
        shader.setMat4("model", model);
        // render Cube
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, numline);
        glBindVertexArray(0);
    }

    Object()
    {
        numline = 0;
    }

    void Load(const char *fileName)
    {
        ifstream fstream(fileName);
        string line;
        vector<float> arr;
        if (fstream)
        {
            while (std::getline(fstream, line))
            {
                float var;
                if (line.compare(0, 1, "/") == 0)
                    continue;
                numline++;
                istringstream ss(line);
                while(ss >> var)
                    arr.push_back(var);
            }
        }
		else
		{
			std::cout << "Can not load the file " << fileName << std::endl;
		}
		fstream.close();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (arr.size() * sizeof(float)), arr.data(), GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~Object() 
    {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }
};

