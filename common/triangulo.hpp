#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>
#include <GL/gl.h>

// Include GLFW
#include <GLFW/glfw3.h>
using namespace glm;

class Triangle
{
    // An array of 3 vectors which represents 3 vertices
public:
    glm::vec3 vertices[3] = {
        vec3(-1, -1, 0),
        vec3(1, -1, 0),
        vec3(0, 1, 0)};
    quat rot = quat(vec3(0,0,0));
    glm::vec3 scale = vec3(1,1,1);
    glm::vec3 pos = vec3(0,0,0);
    void Translate(vec3 movePos){
        pos += movePos*rot;
    }
    void Rotate(vec3 eulerAngles){
        rot = rot*quat(eulerAngles);
    } 
    void PassToBuffer(GLfloat *vertexB, GLfloat *normals){
        PassToBuffer(vertexB);
        vec3 edge1 = vertices[1]*rot-vertices[0]*rot;
        vec3 edge2 = vertices[2]*rot-vertices[0]*rot;
        vec3 normal = normalize(cross(edge1, edge2));
        for (int i = 0; i < 9; i += 3)
        {
            vec3 auxNormal = normal;
            auxNormal = normalize(auxNormal);
            normals[i] = auxNormal.x;
            normals[i + 1] = auxNormal.y;
            normals[i + 2] = auxNormal.z;
        }
    }
    void PassToBuffer(GLfloat *vertexB){
        int j = 0;
        for (int i = 0; i < 9; i += 3)
        {
            vec3 vector = vertices[j]*scale*rot+pos;
            vertexB[i] = vector.x;
            vertexB[i + 1] = vector.y;
            vertexB[i + 2] = vector.z;
            j++;
        }
    }
};