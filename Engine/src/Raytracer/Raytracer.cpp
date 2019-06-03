#include <iostream>
#include <SDL.h>
#include "Raytracer.h"



void Raytracer::draw(const Scene &scene)
{	
	m_num_frames++;
	comp_shader.use();

	//samples
	glUniform1f(numFramesUniform, float(m_num_frames) );
	glUniform1f(accumUniform, float(m_num_frames) / float(m_num_frames + 1));

	glUniformMatrix4fv(viewUniform, 1, GL_FALSE, &camera.calculateInvViewMatrix()[0][0]);
	glUniformMatrix4fv(projUniform, 1, GL_FALSE, &camera.calculateInvProjection()[0][0]);

	/* Bind level 0 of framebuffer texture as writable image in the shader. */
	glBindImageTexture(0, tex, 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);

	/* Compute appropriate invocation dimension. */
	int worksizeX = nextPowerOfTwo(350);
	int worksizeY = nextPowerOfTwo(350);
	
	/* Invoke the compute shader. */
	glDispatchCompute(worksizeX / 8, worksizeY / 8, 1);

	/* Reset image binding. */
	glBindImageTexture(0, 0, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);


	
	m_shader.use();
	//rayTraced.createImage(400, 400);
	glBindVertexArray(m_vao);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUseProgram(m_shader.getHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUniform1i(glGetUniformLocation(m_shader.getHandle(), "tex"), 0);
}



