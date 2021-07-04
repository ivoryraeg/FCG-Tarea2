#include "retuto.hpp"
#include "muro.hpp"
#ifndef PUERTA_H
#define PUERTA_H

class Puerta{
    public:
        Muro *miGrafica;
        int sigEscena;
        Puerta(int sigEscena,vec3 upLeft, vec3 upRight, vec3 downLeft, vec3 downRight, GLuint setTextureIndex);
        void Draw(GLuint programID, GLFWwindow *window);
        void Destroy();
};

#endif