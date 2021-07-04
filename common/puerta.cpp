#include "puerta.hpp"
#include "muro.hpp"


Puerta::Puerta(int sigEscena,vec3 upLeft, vec3 upRight, vec3 downLeft, vec3 downRight, GLuint setTextureIndex){
    this->sigEscena = sigEscena;
    miGrafica = new Muro(upLeft,upRight,downLeft,downRight,setTextureIndex);
} 
void Puerta::Draw(GLuint programID, GLFWwindow *window){
    miGrafica->Draw(programID);
    if(glm::distance(getCameraPosition(),miGrafica->up.pos)<1){
        loadScreen(window, sigEscena);
    }
}

void Puerta::Destroy(){
    miGrafica->Destroy();
    free(miGrafica);
}