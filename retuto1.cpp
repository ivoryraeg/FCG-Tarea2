// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <chrono>
#include <math.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>
#include <GL/gl.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;

#include "common/shader.hpp"

const unsigned long FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
const unsigned long FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
const unsigned long FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))


// Initial cameraPosition : on +Z
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;
GLFWwindow *window;
float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


glm::mat4 getViewMatrix(){
    return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
    return ProjectionMatrix;
}
glm::vec3 cameraPosition;
glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);




double timeToShader = 3;
// This will identify our vertex buffer

int screen = 0;

bool cameraReset = false;
bool cameraResetComeBack = true;

GLuint vertexbuffer[2];
GLuint vertexbufferUI[2];
GLuint uvbufferUI[2];
GLuint programID;
GLuint programID2;
GLuint programUI;
GLuint programID3;
GLuint uvbuffer;
GLuint Texture;
GLuint normalbuffer[2];

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

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

class Muro {
    public:
        Triangle up;
        Triangle down;
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
        Muro(vec3 upLeft, vec3 upRight, vec3 downLeft, vec3 downRight) {
            up.vertices[0] = downLeft;
            up.vertices[1] = upLeft;
            up.vertices[2] = upRight;

            down.vertices[0] = downLeft;
            down.vertices[1] = upRight;
            down.vertices[2] = downRight;

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

        void Draw() {
            //glUseProgram(programUI);
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
};

Triangle triangle1, triangle2;


static GLfloat g_vertex_buffer_data1[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
    };
static GLfloat g_vertex_buffer_data2[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
    };

static GLfloat g_vertex_buffer_dataUI1[] = {
    -1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
};
static GLfloat g_vertex_buffer_dataUI2[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f
};

static const GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f
};

static const GLfloat g_uv_buffer_data[] = {
    0.0f, 1.0f-0.0f,
    1.0f, 1.0f-0.0f,
    1.0f, 1.0f-1.0f
};
static const GLfloat g_uv_buffer_dataUI1[] = { 
    0.0f, 1.0f-0.0f,
    0.0f, 1.0f-1.0f,
    1.0f, 1.0f-1.0f
};
static const GLfloat g_uv_buffer_dataUI2[] = {
    0.0f, 1.0f-0.0f,
    1.0f, 1.0f-1.0f,
    1.0f, 1.0f-0.0f
};

static GLfloat g_normal_buffer_data1[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
};

static GLfloat g_normal_buffer_data2[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
};



GLuint loadDDS(const char * imagepath){

    unsigned char header[124];

    FILE *fp;

    /* try to open the file */
    fp = fopen(imagepath, "rb");
    if (fp == NULL){
        printf("error 1\n");
        return 0;
    }

    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);
        printf("error 2\n");
        return 0;
    }

    /* get the surface desc */
    fread(&header, 124, 1, fp); 

    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width       = *(unsigned int*)&(header[12]);
    unsigned int linearSize  = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);
    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);
        unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        free(buffer);
        printf("error 3\n");
        return 0;
    }
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable( GL_TEXTURE_2D );
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 
            0, size, buffer + offset);

        offset += size;
        width  /= 2;
        height /= 2;
    }
    free(buffer); 

    return textureID;
}

void computeMatricesFromInputs(){


	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse cameraPosition
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse cameraPosition for next frame
	glfwSetCursorPos(window, 768/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(768/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		cameraPosition += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		cameraPosition -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		cameraPosition += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		cameraPosition -= right * deltaTime * speed;
	}
    
	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								cameraPosition,           // Camera is here
								cameraPosition+direction, // and looks here : at the same cameraPosition, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void ControlProcessing()
{


    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint MatrixIDM = glGetUniformLocation(programID, "M");
    GLuint MatrixIDV = glGetUniformLocation(programID, "V");

    computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(MatrixIDM, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &ViewMatrix[0][0]);

}



void Scene1(double deltaTime,GLFWwindow *window)
{
   
    glUseProgram(programUI);
    static float yDirection = 1;
    // Draw triangle...

    for(int i=0;i<2;i++){
        auto g_vertex = g_vertex_buffer_dataUI1;
        auto g_uv = g_uv_buffer_dataUI1;
        if(i==1){
            g_vertex = g_vertex_buffer_dataUI2;
            g_uv = g_uv_buffer_dataUI2; 
        }

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferUI[i]);
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
        glBindBuffer(GL_ARRAY_BUFFER, uvbufferUI[i]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv), g_uv, GL_STATIC_DRAW);
        glVertexAttribPointer(
            1,        // attribute 1. No particular reason for 0, but must match the layout in the shader.
            2,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );    
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }    
}

void loadScreen(GLFWwindow *window, int scene){

    auto t_startLoad = std::chrono::high_resolution_clock::now();
    // the work...
    auto t_endLoad = std::chrono::high_resolution_clock::now();

    double deltaTimeLoad = 0;

    float cont = 2;

    if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
        if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE){

            std::cout << "Hola" << std::endl;
            while(cont > 0){
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glUniform1i(glGetUniformLocation(programUI,"myTextureSampler"),1);
                Scene1(deltaTimeLoad,window);

                t_startLoad = t_endLoad;
                t_endLoad = std::chrono::high_resolution_clock::now();
                //std::cout << cont << std::endl;
                deltaTimeLoad = std::chrono::duration<double>(t_endLoad - t_startLoad).count();
                cont -= deltaTimeLoad;
                if(cont <= 0)
                    {
                        screen = scene;
                    }
                
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
            std::cout << "Adios" << std::endl;  
        }
    }
}


void Scene2(double deltaTime, GLFWwindow *window)
{
    glUseProgram(programID);
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DRAW_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);     
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
 
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

     //3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[0]);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
        
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        glUniform1i(glGetUniformLocation(programID,"myTextureSampler"),0);
    }
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        glUniform1i(glGetUniformLocation(programID,"myTextureSampler"),1);
    }  
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
        glUniform1i(glGetUniformLocation(programID,"myTextureSampler"),2);
    }

    // Draw the triangle !
      
    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glUseProgram(programID2);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]); 
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
       //3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[1]);
		glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	
    );
        
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        glUniform1i(glGetUniformLocation(programID2,"myTextureSampler"),0);
    }
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        glUniform1i(glGetUniformLocation(programID2,"myTextureSampler"),1);
    }  
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
        glUniform1i(glGetUniformLocation(programID2,"myTextureSampler"),2);
    }
    // Draw the triangle !
       
    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);


    // Don't forget to #include <glm/gtc/quaternion.hpp> and <glm/gtx/quaternion.hpp>
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){//Sube
            triangle1.Translate(vec3(0,deltaTime,0));
        }   
        else{
            triangle1.Translate(vec3(-deltaTime,0,0));
        }
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){

        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){//Baja
            triangle1.Translate(vec3(0,-deltaTime,0));
        }   
        else{
            triangle1.Translate(vec3(deltaTime,0,0));
        }
    }
    //Escala en x
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
        
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){//Lo hace en sentido contrario
            triangle1.scale += vec3(-deltaTime,0,0);
        }   
        else{
            triangle1.scale += vec3(+deltaTime,0,0);
        }
    }
    //Escala en y
    if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS){
        
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){//Lo hace en sentido contrario
            triangle1.scale += vec3(0,-deltaTime,0);
        }   
        else{
            triangle1.scale += vec3(0,+deltaTime,0);
        }
    }
    //Escala en z
    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
        
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){//Lo hace en sentido contrario
            triangle1.scale += vec3(0,0,-deltaTime);
        }   
        else{
            triangle1.scale += vec3(0,0,+deltaTime);
        }
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){//Rota triangulo 1 hacia izq
        triangle1.Rotate(vec3(0,0,deltaTime));
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){//Rota triangulo 1 hacia der
        triangle1.Rotate(vec3(0,0,-deltaTime));
    }
    triangle2.Rotate(vec3(0,0,-deltaTime));//Rota triangulo 2 hacia izq automaticamente
    triangle1.PassToBuffer(g_vertex_buffer_data1);
    triangle2.PassToBuffer(g_vertex_buffer_data2);

   
    
}

void Scene3(double deltaTime,GLFWwindow *window) {


    ControlProcessing();
    
    glUseProgram(programID3);
    static float yDirection = 1;
    static Muro *muro = new Muro(vec3(-1, 1, 0), vec3(1, 1, 0), vec3(-1, -1, 0), vec3(1, -1, 0));
    static Muro *muro2 = new Muro(vec3(1, 1, 0), vec3(1, 1, -5), vec3(1, -1, 0), vec3(1, -1, -5));
    static Muro *muro3 = new Muro(/*triangulo izq arriba*/vec3(-1, 1, -5), /*triangulo der arriba*/vec3(-1, 1, 0),/*triangulo izq abajo*/ vec3(-1, -1, -5),/*triangulo der abajo*/ vec3(-1, -1, 0));
    static Muro *Suelo = new Muro(vec3(-1, -1, -5), vec3(1, -1, -5), vec3(-1, -1, 0), vec3(1, -1, 0));
    static Muro *Techo = new Muro(vec3(-1, 1, -5), vec3(1, 1, -5), vec3(-1, 1, 0), vec3(1, 1, 0));
    static Muro *Puerta = new Muro(vec3(1, 1, -5), vec3(-1, 1, -5), vec3(1, -1, -5), vec3(-1, -1, -5));
    static Muro *Cuadro = new Muro(vec3(0.95, 0.5, -0.5), vec3(0.95, 0.5, -1.5), vec3(0.95, -0.5, -0.5), vec3(0.95, -0.5, -1.5));
    static Muro *Ventana = new Muro(vec3(-0.95, 0.5, -1.5), vec3(-0.95, 0.5, -3.5), vec3(-0.95, -0.5, -1.5), vec3(-0.95, -0.5, -3.5));

    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),2);
    muro->Draw();
    muro2->Draw();
    muro3->Draw();
    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),3);
    Puerta->Draw();
    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),5);
    Cuadro->Draw();
    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),6);
    Ventana->Draw();
    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),7);
    Suelo->Draw();
    Techo->Draw();
    //muro->up.Rotate(vec3(0.0,deltaTime,0.0));
    //muro->down.Rotate(vec3(0.0,deltaTime,0.0));

    if(!cameraReset)
    {
        cameraReset = true;
        cameraPosition = vec3(0,0,0);
    }
    if(!cameraResetComeBack)
    {
        cameraResetComeBack = true;
        cameraPosition = vec3(0,0,-4.5);
    }

    if(cameraPosition.z <= -5)
    {
        cameraResetComeBack = false;
        cameraReset = false;
        screen = 2;
    }
    if(cameraPosition.z > 0)
    {
        cameraPosition.z = 0;
    }
    if(cameraPosition.x > 1)
    {
        cameraPosition.x = 1;
    }
    if(cameraPosition.x < -1)
    {
        cameraPosition.x = -1;
    }
    if(cameraPosition.y > 1)
    {
        cameraPosition.y = 1;
    }
    if(cameraPosition.y < -1)
    {
        cameraPosition.y = -1;
    }
    
}

void Scene4(double deltaTime,GLFWwindow *window) {

    ControlProcessing();
    
    glUseProgram(programID3);
    static float yDirection = 1;
    static Muro *Puerta = new Muro(vec3(-1, 1, 0), vec3(1, 1, 0), vec3(-1, -1, 0), vec3(1, -1, 0));
    static Muro *muro = new Muro(vec3(1, 1, 0), vec3(1, 1, -2), vec3(1, -1, 0), vec3(1, -1, -2));
    static Muro *muro2 = new Muro(/*triangulo izq arriba*/vec3(-1, 1, -2), /*triangulo der arriba*/vec3(-1, 1, 0),/*triangulo izq abajo*/ vec3(-1, -1, -2),/*triangulo der abajo*/ vec3(-1, -1, 0));
    static Muro *muro3 = new Muro(vec3(1, 1, -2), vec3(-1, 1, -2), vec3(1, -1, -2), vec3(-1, -1, -2));
    static Muro *Suelo = new Muro(vec3(-1, -1, -2), vec3(1, -1, -2), vec3(-1, -1, 0), vec3(1, -1, 0));
    static Muro *Techo = new Muro(vec3(-1, 1, -2), vec3(1, 1, -2), vec3(-1, 1, 0), vec3(1, 1, 0));
    static Muro *Cuadro = new Muro(vec3(0.95, 0.5, -0.5), vec3(0.95, 0.5, -1.5), vec3(0.95, -0.5, -0.5), vec3(0.95, -0.5, -1.5));
    static Muro *Estanteria = new Muro(vec3(-0.95, 0.5, -0.5), vec3(-0.95, 0.5, -1.5), vec3(-0.95, -1, -0.5), vec3(-0.95, -1, -1.5));

    

    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),4);
    muro->Draw();
    muro2->Draw();
    muro3->Draw();
    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),3);
    Puerta->Draw();
    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),7);
    Suelo->Draw();
    Techo->Draw();
    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),8);
    Cuadro->Draw();
    glUniform1i(glGetUniformLocation(programID3,"myTextureSampler"),9);
    Estanteria->Draw();
    //muro->up.Rotate(vec3(0.0,deltaTime,0.0));
    //muro->down.Rotate(vec3(0.0,deltaTime,0.0));


    if(!cameraReset)
    {
        cameraReset = true;
        cameraPosition = vec3(0,0,-0.5);
    }
    if(cameraPosition.z > 0)
    {
        cameraResetComeBack = false;
        screen = 3;
    }
    if(cameraPosition.z < -2)
    {
        cameraPosition.z = -2;
    }
    if(cameraPosition.x > 1)
    {
        cameraPosition.x = 1;
    }
    if(cameraPosition.x < -1)
    {
        cameraPosition.x = -1;
    }
    if(cameraPosition.y > 1)
    {
        cameraPosition.y = 1;
    }
    if(cameraPosition.y < -1)
    {
        cameraPosition.y = -1;
    }

}




int main()
{

    // Initialise GLFW
    glewExperimental = true; // Needed for core profile
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(768, 768, "Tutorial 01", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental = true;        // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 768/2, 768/2);


    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
    programID2 = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

    programUI = LoadShaders( "MyVertex3.shader", "MyFragmentBK.shader" );

    programID3 = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
    // Use our shader
    glUseProgram(programID);

    GLuint VertexArrayID[2];
    glGenVertexArrays(2, VertexArrayID);
    glBindVertexArray(VertexArrayID[0]);//*
    glBindVertexArray(VertexArrayID[1]);//*

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(2, vertexbuffer);
    // Give our vertices to OpenGL.
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
    glNamedBufferData(VertexArrayID[0], sizeof(g_vertex_buffer_data1), g_vertex_buffer_data1, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);    
    glNamedBufferData(VertexArrayID[1], sizeof(g_vertex_buffer_data2), g_vertex_buffer_data2, GL_STATIC_DRAW);

    GLuint VertexArrayIDUI[2];
    // Generate 2 buffer, put the resulting identifier in vertexbuffer
    glGenVertexArrays(2, VertexArrayIDUI);
    glGenBuffers(2, vertexbufferUI);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferUI[0]);        
    glNamedBufferData(VertexArrayIDUI[0], sizeof(g_vertex_buffer_dataUI1), g_vertex_buffer_dataUI1, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferUI[1]);    
    glNamedBufferData(VertexArrayIDUI[1], sizeof(g_vertex_buffer_dataUI2), g_vertex_buffer_dataUI2, GL_STATIC_DRAW);

    glUseProgram(programUI);

    GLuint unit = 0;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("Pantalla_Inicial.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);    
    
    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("LoadingScreen.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);
    
    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("Madera.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);

    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("Puerta.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);

    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("Ladrillo.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);

    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("Cuadro.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);

    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("Ventana.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);

    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("TexturaTablones.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);

    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("Cuadro2.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);

    unit++;
    glActiveTexture(GL_TEXTURE0 + unit);
    Texture = loadDDS("Estanteria.DDS");
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), unit);

    glGenBuffers(2, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(2, normalbuffer);

    glGenBuffers(2, uvbufferUI);
    glBindBuffer(GL_ARRAY_BUFFER, uvbufferUI[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_dataUI1), g_uv_buffer_dataUI1, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, uvbufferUI[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_dataUI2), g_uv_buffer_dataUI2, GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[0]);
    glNamedBufferData(normalbuffer[0], sizeof(g_normal_buffer_data1), g_normal_buffer_data1, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[1]);
    glNamedBufferData(normalbuffer[1], sizeof(g_normal_buffer_data2), g_normal_buffer_data2, GL_STATIC_DRAW);

    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    GLuint LightID2 = glGetUniformLocation(programID2, "LightPosition_worldspace");
    GLuint LightID3 = glGetUniformLocation(programID3, "LightPosition_worldspace");

    auto t_start = std::chrono::high_resolution_clock::now();
    // the work...
    auto t_end = std::chrono::high_resolution_clock::now();

    double deltaTime = 0;
    triangle1.pos = vec3(0,0,0);
    triangle2.pos = vec3(.5,0,0);

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");
    GLuint ViewMatrixID2 = glGetUniformLocation(programID2, "V");
    GLuint ModelMatrixID2 = glGetUniformLocation(programID2, "M");

    GLuint MatrixID3 = glGetUniformLocation(programID3, "MVP");
    GLuint ViewMatrixID3 = glGetUniformLocation(programID3, "V");
    GLuint ModelMatrixID3 = glGetUniformLocation(programID3, "M");



    do
    {


        
        // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	    glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        // Camera matrix
        glm::mat4 ViewMatrix       = glm::lookAt(
                                    glm::vec3 (-3,0,-5),            // Camera is here
                                    glm::vec3 (0,0,0),            // and looks here : at the same position, plus "direction"
                                    glm::vec3 (0,1,0)             // Head is up (set to 0,-1,0 to look upside-down)
                            );
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        
        //Get a handle for our "MVP" uniform
        glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID3, 1, GL_FALSE, &ViewMatrix[0][0]);

        //glUseProgram(programID);

        //getsTime Dif
        t_start = t_end;
        t_end = std::chrono::high_resolution_clock::now();

        deltaTime = std::chrono::duration<double>(t_end - t_start).count();

        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Draw through function
        if(screen == 0){
            loadScreen(window, 1);
            glUseProgram(programUI);
            glUniform1i(glGetUniformLocation(programUI, "myTextureSampler"), 0);  
            Scene1(deltaTime, window);
        }else if(screen == 1){
            loadScreen(window, 0);

            glUseProgram(programID3);
            glUniform3f(LightID3, 3,2,-3);
            glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID3, 1, GL_FALSE, &ViewMatrix[0][0]);

            Scene3(deltaTime, window);
        }
        else if(screen == 2){

            glUseProgram(programID3);
            glUniform3f(LightID3, 3,2,-3);
            glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID3, 1, GL_FALSE, &ViewMatrix[0][0]);

            Scene4(deltaTime, window);
        }
        else if(screen == 3){

            glUseProgram(programID3);
            glUniform3f(LightID3, 3,2,-3);
            glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID3, 1, GL_FALSE, &ViewMatrix[0][0]);

            Scene3(deltaTime, window);
        }

        glNamedBufferData(VertexArrayID[0], sizeof(g_vertex_buffer_data1), g_vertex_buffer_data1, GL_STATIC_DRAW);        
        glUseProgram(programID2);
        glNamedBufferData(VertexArrayID[1], sizeof(g_vertex_buffer_data2), g_vertex_buffer_data2, GL_STATIC_DRAW);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
}