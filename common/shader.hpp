// Include standard headers
#include <stdio.h>
#include <stdlib.h>
//#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);