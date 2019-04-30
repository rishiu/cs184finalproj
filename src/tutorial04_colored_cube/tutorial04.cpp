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
#include <chrono>
#include <thread>

struct Particle{
    vec3 vel;
    vec4 pos, color;
    GLfloat temp;
    
    Particle(vec4 p, vec3 v, vec4 c, GLfloat temp): vel(v), pos(p), color(c), temp(temp) { }
};

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
	window = glfwCreateWindow( 1024, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    
    //GLuint lightpos = glGetUniformLocation(programID, "u_light_pos");
    //GLuint lightin = glGetUniformLocation(programID, "u_light_intensity");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(4,1,-3), // Camera is at (4,3,-3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

    vec4 heatpos = vec4(0,5,0,1); //In world coordinates
    vec4 lightintensity = vec4(0.5,0.5,0.5,0.1);
    //For early revisions I assume the heat source is the same size as a face of the cube.
    
    std::vector<Particle> particles;
    
    std::vector<vec3> facenormals;
    
    
    for(int i = -1; i < 2; i+=2){
        facenormals.push_back(vec3(0,i,0));
        facenormals.push_back(vec3(i,0,0));
        facenormals.push_back(vec3(0,0,i));
    }
    
    static GLfloat g_vertex_buffer_data[21654936];
    static GLfloat g_color_buffer_data[21654936];
                
    int index = 0;
    for(GLfloat x = -0.5f; x < 0.5f; x+=0.0075f){
        for(GLfloat y = -0.5f; y < 0.5f; y+=0.0075f){
            for(GLfloat z = -0.5f; z < 0.5f; z+=0.0075f){
                vec4 p = vec4(x, y, z, 1.0f);
                vec3 v = vec3(0,0,0);
                vec4 c = vec4(1,1,1,1);
                Particle pa = Particle(p, v, c, 400.0f);
                particles.push_back(pa);
                g_vertex_buffer_data[index] = pa.pos.x;
                g_vertex_buffer_data[index + 1] = pa.pos.y;
                g_vertex_buffer_data[index + 2] = pa.pos.z;
                g_vertex_buffer_data[index + 3] = pa.pos.x + 0.0075;
                g_vertex_buffer_data[index + 4] = pa.pos.y;
                g_vertex_buffer_data[index + 5] = pa.pos.z;
                g_vertex_buffer_data[index + 6] = pa.pos.x;
                g_vertex_buffer_data[index + 7] = pa.pos.y + 0.0075;
                g_vertex_buffer_data[index + 8] = pa.pos.z;
                g_color_buffer_data[index] = 0.0f;
                g_color_buffer_data[index + 1] = 0.0f;
                g_color_buffer_data[index + 2] = 0.0f;
                g_color_buffer_data[index + 3] = 0.0f;
                g_color_buffer_data[index + 4] = 0.0f;
                g_color_buffer_data[index + 5] = 0.0f;
                g_color_buffer_data[index + 6] = 0.0f;
                g_color_buffer_data[index + 7] = 0.0f;
                g_color_buffer_data[index + 8] = 0.0f;
                index += 9;
            }
        }
    }
    
    std::cout << particles.size() << std::endl;
    
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_DYNAMIC_DRAW);

    int c = 0;
	do{
        //std::cout << "o" << std::endl;
        GLfloat sig = 5.67E-8;
        GLfloat Theat = 5500.0f;
        //GLfloat Tobject = 400.0f;
        GLfloat F = 1.0f;
        GLfloat A = 0.784f;
        //std::cout << q << std::endl;
        
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        for(Particle &p : particles){
            GLfloat Tobject = p.temp;
            GLfloat q = sig * F * A * (pow(Theat, 4) - pow(Tobject, 4));
            GLfloat parea = 0.784 / pow(66, 2);
            GLfloat qp = parea * q;
            if(p.pos.y >= -0.5 + 62 * 0.015){
                GLfloat dist = -0.5 + 60 * 0.015 - p.pos.y;
                GLfloat c = 460.548;
                GLfloat dT = (qp / c) * (1 - dist);
                //std::cout << dT << std::endl;
                //std::cout << p.temp << std::endl;
                p.temp = p.temp + dT;
                //std::cout << p.temp << std::endl;
            }else{
                p.temp = 400.0f;
            }
        }
        
        vec4 iron = vec4(0.501,0.501,0.501,1.0);
        vec4 red = vec4(1.0,0,0,1);
        vec4 reddishorange = vec4(1.0, 0.27, 0, 1);
        vec4 yellowishorange = vec4(1.0, 0.682, 0.259, 1);
        vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
        vec4 yellowishwhite = vec4(1.0, 1.0, 0.5, 1.0);
        vec4 warmwhite = vec4(1.0, 1.0, 0.8, 1.0);
        vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
        int index = 0;
        for(Particle &p : particles){
            vec4 color = vec4();
            //std::cout << p.temp << std::endl;
            if(p.temp < 1000.0){
                GLfloat diff = (p.temp - 400.0)/(1000.0 - 400.0);
                color = ((1 - diff) * iron) + (diff * red);
            }else if(p.temp > 1000.0 && p.temp <= 1500.0){
                GLfloat diff = (p.temp - 1000.0)/(1500.0 - 1000.0);
                color = ((1 - diff) * red) + (diff * reddishorange) + 0.01f * iron;
            }else if(p.temp > 1500.0 && p.temp <= 2000.0){
                GLfloat diff = (p.temp - 1500.0)/(2000.0 - 1500.0);
                color = ((1 - diff) * reddishorange) + (diff * yellowishorange);
            }else if(p.temp >= 2000.0 && p.temp <= 2800.0){
                GLfloat diff = (p.temp - 2000.0)/(2800.0 - 2000.0);
                color = ((1 - diff) * yellowishorange) + (diff * yellow);
            }else if(p.temp >= 2800.0 && p.temp <= 3500.0){
                GLfloat diff = (p.temp - 2800.0)/(3500.0 - 2800.0);
                color = ((1 - diff) * yellow) + (diff * yellowishwhite);
            }else if(p.temp >= 3500.0 && p.temp <= 4500.0){
                GLfloat diff = (p.temp - 3500.0)/(4500.0 - 3500.0);
                color = ((1 - diff) * yellowishwhite) + (diff * warmwhite);
            }else if(p.temp >= 4500.0 && p.temp <= 5500.0){
                GLfloat diff = (p.temp - 4500.0)/(5500.0 - 4500.0);
                color = ((1 - diff) * warmwhite) + (diff * white);
            }
            //std::cout << color.x << std::endl;
            g_color_buffer_data[index] = color.x;
            g_color_buffer_data[index + 1] = color.y;
            g_color_buffer_data[index + 2] = color.z;
            g_color_buffer_data[index + 3] = color.x;
            g_color_buffer_data[index + 4] = color.y;
            g_color_buffer_data[index + 5] = color.z;
            g_color_buffer_data[index + 6] = color.x;
            g_color_buffer_data[index + 7] = color.y;
            g_color_buffer_data[index + 8] = color.z;
            index += 9;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        
        //glUniform4fv(lightpos, 1, heatpos);
        //glUniform4fv(lightin, 1, lightintensity);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
        
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 21654936); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        c++;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	//glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

