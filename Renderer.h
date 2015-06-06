//Jeremy Brooks - Copyright 2008.
//Please do not copy or use without permission

#ifndef _RENDERER_H_
#define _RENDERER_H_

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

#include <GLee.h>
#include <SDL.h>

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void DrawFrame(float zoom, float center_x, float center_y, bool is_interactive, int max_iterations, int& iteration);

private:
	void DrawFullScreenQuad(float min_x, float max_x, float min_y, float max_y);

	void Init();

	GLuint fbo;
	GLuint input_texture[2];

	GLuint uniform_iteration_location;
	GLuint uniform_max_iterations_location;
	GLuint uniform_input_p2_location;
	GLuint uniform_input_p3_location;
	GLuint uniform_iteration_scale_location;

	GLenum program_pass1;
	GLenum program_pass2;
	GLenum program_pass3;

	bool ping;//pong, used to flip inputs between shaders
};

#endif