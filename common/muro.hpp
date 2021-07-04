#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>
#include <GL/gl.h>

// Include GLFW
#include <GLFW/glfw3.h>

#ifndef MURO_H
#define MURO_H
#include "triangulo.hpp"
class Muro {
    public:
        Triangle up;
        Triangle down;
        GLuint textureIndex;
        GLuint vertexBuffersID[2];
        GLuint normalBuffersID[2];
        GLuint UVBuffersID[2];
        GLfloat vertexBufferUp[9];
        GLfloat vertexBufferDown[9];
        GLfloat normalBufferUp[9];
        GLfloat normalBufferDown[9];
        GLfloat UVBufferUp[6] = { 
            0.0f, 1.0f-0.0f,
            0.0f, 1.0f-1.0f,
            1.0f, 1.0f-1.0f
        };
        GLfloat UVBufferDown[6] = { 
            0.0f, 1.0f-0.0f,
            1.0f, 1.0f-1.0f,
            1.0f, 1.0f-0.0f
        };
        Muro(vec3 upLeft, vec3 upRight, vec3 downLeft, vec3 downRight, GLuint setTextureIndex);

        void Draw(GLuint programID);
        void Destroy();        
};
#endif