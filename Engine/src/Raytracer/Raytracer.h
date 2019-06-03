#pragma once

#include "../Model.h"
#include "../Scene.h"
#include "../Application.h"
#include "../ShaderProgram.h"
#include "../../TexCreate.h"


class Raytracer {
public:
	// constructor
	Raytracer(int h, int w, RayCamera &cam) {
		// create quad on which to render scene
		//m_vertexNum = 6;

		camera = cam;

		GLfloat vertices[] = {
			// positions			  // colors           // texture coords
			-1.0, 1.0f, 0.0f,		 1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left
			1.0f, -1.0f, 0.0f,		 0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
			-1.0f, -1.0f, 0.0f,		 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
			1.0f, 1.0f, 0.0f,		 1.0f, 0.0f, 0.0f,   1.0f, 1.0f   // top right
		};

		GLuint indices[] = { 0, 1, 2, 0, 1, 3 };

		
		// generate vertex buffer object (allocates memory on Graphics card)
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// vertex array object, which holds information about the buffer. This is what is used to draw the genoetry, not the vertex buffer.
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// indices
		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		// create shader
		m_shader = ShaderProgram::ShaderProgram();
		m_shader.loadShaders("./resources/shaders/raytracer.vert", "./resources/shaders/raytracer.frag");




		

		//create texture to store data in

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		GLubyte black = NULL;
		

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 350, 350, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);


		//compile compute shader
		comp_shader = ShaderProgram::ShaderProgram();
		comp_shader.loadComputeShader("./resources/shaders/compute.frag");

		//initialize compute shader
		glUseProgram(comp_shader.getCompHandle());
		int *workGroupSize = new int();
		
		glGetProgramiv(comp_shader.getCompHandle(),GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
		workGroupSizeX = workGroupSize[0];
		cout << "work group size x :" << workGroupSizeX << endl;
		
		projUniform = glGetUniformLocation(comp_shader.getCompHandle(), "InvProjectionMat"); 
		viewUniform = glGetUniformLocation(comp_shader.getCompHandle(), "InvViewMat");
		numFramesUniform = glGetUniformLocation(comp_shader.getCompHandle(), "num_frames");
		accumUniform = glGetUniformLocation(comp_shader.getCompHandle(), "accum");
		glUseProgram(0);
		m_num_frames = 1;


		rayTraced = TexCreate();
		rayTraced.createWorld();
		rayTraced.setCamera(camera);

	}

	// methods
	void draw(const Scene &scene);
	inline TexCreate getTex() { return rayTraced; }
	inline void setRayCamera(RayCamera &cam) { camera = cam; rayTraced.setCamera(cam); }
	inline void reset() { m_num_frames = 0; }

	TexCreate rayTraced;
private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ibo;
	ShaderProgram m_shader;

	ShaderProgram comp_shader;
	vec3 eyeray;
	GLuint tex;
	RayCamera camera;
	
	int viewUniform;
	int projUniform;
	int numFramesUniform;
	int accumUniform;

	int32_t m_num_frames;

	int workGroupSizeX;
	int workGroupSizeY;
	
};

static int nextPowerOfTwo(int x) {
	x--;
	x |= x >> 1; // handle 2 bit numbers
	x |= x >> 2; // handle 4 bit numbers
	x |= x >> 4; // handle 8 bit numbers
	x |= x >> 8; // handle 16 bit numbers
	x |= x >> 16; // handle 32 bit numbers
	x++;
	return x;
}