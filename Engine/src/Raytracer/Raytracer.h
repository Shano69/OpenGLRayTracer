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

		//MY CODE

		rayTraced = TexCreate();
		rayTraced.setCamera(camera);

	}

	// methods
	void draw(const Scene &scene);
	inline TexCreate getTex() { return rayTraced; }
	inline void setRayCamera(RayCamera &cam) { camera = cam; rayTraced.setCamera(cam); }

	TexCreate rayTraced;
private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ibo;
	ShaderProgram m_shader;

	//my stuff
	
	RayCamera camera;
	
};