#include <iostream>
#include <SDL.h>
#include "Raytracer.h"



void Raytracer::draw(const Scene &scene)
{
	
	
	
	m_shader.use();
	rayTraced.createImage(400, 400);
	glBindVertexArray(m_vao);
	rayTraced.bind();
	glUseProgram(m_shader.getHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUniform1i(glGetUniformLocation(m_shader.getHandle(), "tex"), 0);
}



