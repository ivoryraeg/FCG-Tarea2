#include "muro.hpp"

Muro::Muro(vec3 upLeft, vec3 upRight, vec3 downLeft, vec3 downRight, GLuint setTextureIndex) {
    up.vertices[0] = downLeft;
    up.vertices[1] = upLeft;
    up.vertices[2] = upRight;

    down.vertices[0] = downLeft;
    down.vertices[1] = upRight;
    down.vertices[2] = downRight;


    up.PassToBuffer(vertexBufferUp, normalBufferUp);
    down.PassToBuffer(vertexBufferDown, normalBufferDown);

    textureIndex = setTextureIndex;
    glGenBuffers(2, UVBuffersID);
    glBindBuffer(GL_ARRAY_BUFFER, UVBuffersID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UVBufferUp), UVBufferUp, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, UVBuffersID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UVBufferDown), UVBufferDown, GL_STATIC_DRAW);

    glGenBuffers(2, normalBuffersID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffersID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normalBufferUp), normalBufferUp, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffersID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normalBufferUp), normalBufferUp, GL_STATIC_DRAW);

    glGenBuffers(2, vertexBuffersID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffersID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferUp), vertexBufferUp, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffersID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferDown), vertexBufferDown, GL_STATIC_DRAW);                    
}

void Muro::Draw(GLuint programID) {
    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID,"myTextureSampler"),textureIndex);
    // Draw triangle...
    up.PassToBuffer(vertexBufferUp, normalBufferUp);
    down.PassToBuffer(vertexBufferDown, normalBufferDown);

    glBindBuffer(GL_ARRAY_BUFFER, UVBuffersID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UVBufferUp), UVBufferUp, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, UVBuffersID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UVBufferDown), UVBufferDown, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffersID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normalBufferUp), normalBufferUp, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffersID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normalBufferUp), normalBufferUp, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffersID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferUp), vertexBufferUp, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffersID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferDown), vertexBufferDown, GL_STATIC_DRAW);

    for(int i=0;i<2;i++){
        
        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffersID[i]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex), g_vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(
            0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );    
        
        // 2st attribute buffer : uv
        glEnableVertexAttribArray(1);        
        glBindBuffer(GL_ARRAY_BUFFER, UVBuffersID[i]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv), g_uv, GL_STATIC_DRAW);
        glVertexAttribPointer(
            1,        // attribute 1. No particular reason for 0, but must match the layout in the shader.
            2,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );    
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffersID[i]);
        //Normals
        glVertexAttribPointer(
            2,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );   
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }    
}
void Muro::Destroy(){
    glDeleteBuffers(2, UVBuffersID);
    glDeleteBuffers(2, vertexBuffersID);
    glDeleteBuffers(2, normalBuffersID);
    
}