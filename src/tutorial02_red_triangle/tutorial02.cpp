// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <vector>
#include <iostream>

struct Particle{
    vec3 vel;
    vec4 pos, color;
    GLfloat temp;
    
    Particle(vec4 p, vec3 v, vec4 c): vel(v), pos(p), color(c) { }
};

mat4x4 lookAt(vec3 eye, vec3 center, vec3 up){
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    
    vec4 c0 = vec4(s.x, s.y, s.z, -dot(s, eye));
    vec4 c1 = vec4(u.x, u.y, u.z, -dot(u, eye));
    vec4 c2 = vec4(-f.x, -f.y, -f.z, dot(f, eye));
    vec4 c3 = vec4(1.0,1.0,1.0,1.0);
    
    mat4x4 v = mat4x4(c0, c1, c2, c3);
    return v;
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	//glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    //gluLookAt(-0.25, 0.25, -0.25, 0.25, 0.25, 0.25, 0, 1, 0);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
    
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");


    std::vector<Particle> particles;
    
    for(GLfloat x = -1.0f; x < 1.0f; x+=0.2f){
        for(GLfloat y = -1.0f; y < 1.0f; y+= 0.2f){
            for(GLfloat z = -1.0f; z < 1.0f; z+=0.2f){
                vec4 p = vec4(x, y, z, 1.0f);
                vec3 v = vec3(0,0,0);
                vec4 c = vec4(1,1,1,1);
                Particle pa = Particle(p, v, c);
                particles.push_back(pa);
            }
        }
    }
    
    static GLfloat g_vertex_buffer_data[11979];
    
    mat4 Projection = glm::perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    mat4 View = lookAt(glm::vec3(4,3,-3), glm::vec3(0,0,0),  glm::vec3(0,1,0));
    // Model matrix : an identity matrix (model will be at the origin)
    mat4 Model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    mat4 MVP = Projection * View * Model;
    
    //mat4x4 view = lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.75f, 0.75f, 0.75f), vec3(0,1,0));
    int index = 0;
    for(Particle p : particles){
        vec4 po = p.pos;
        std::cout << po.x << std::endl;
        g_vertex_buffer_data[index] = po.x;
        g_vertex_buffer_data[index + 1] = po.y;
        g_vertex_buffer_data[index + 2] = po.z;
        g_vertex_buffer_data[index + 3] = po.x + 0.01;
        g_vertex_buffer_data[index + 4] = po.y;
        g_vertex_buffer_data[index + 5] = po.z;
        g_vertex_buffer_data[index + 6] = po.x;
        g_vertex_buffer_data[index + 7] = po.y + 0.01;
        g_vertex_buffer_data[index + 8] = po.z;
        index += 9;
    }
    

	/*static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         0.0f, -1.0f, 0.0f
	};*/
    
    /*static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };*/
    
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// Use our shader
		glUseProgram(programID);
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3993); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

