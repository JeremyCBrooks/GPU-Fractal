//Jeremy Brooks - Copyright 2008.
//Please do not copy or use without permission

#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Renderer.h"
#include <gl/glu.h>

extern std::ofstream log_file;//external log file declared in main

Renderer::Renderer():ping(true)
{
	//setup OpenGL parameters
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );

	//define video settings
	if(!SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_OPENGL))//|SDL_FULLSCREEN))// | 
	{
		throw std::exception("SDL_SetVideoMode failed.");
	}

	log_file << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	log_file << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	log_file << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	int value;
	log_file << "Display parameters: " << std::endl;
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
	log_file << "SDL_GL_RED_SIZE = " << value << std::endl;
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE,  &value);
	log_file << "SDL_GL_GREEN_SIZE = " << value << std::endl;
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
	log_file << "SDL_GL_BLUE_SIZE = " << value << std::endl;
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &value);
	log_file << "SDL_GL_ALPHA_SIZE = " << value << std::endl;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
	log_file << "SDL_GL_DEPTH_SIZE = " << value << std::endl;
    SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &value);
	log_file << "SDL_GL_BUFFER_SIZE = " << value << std::endl;
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value);
	log_file << "SDL_GL_STENCIL_SIZE = " << value << std::endl;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
	log_file << "SDL_GL_DOUBLEBUFFER = " << value << std::endl;

	glClearColor(0.0,0.0,0.0,1.0);
	//setup OpenGL
	glDisable(GL_LIGHTING);

	glShadeModel(GL_FLAT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	Init();
}

Renderer::~Renderer()
{
	glUseProgramObjectARB(0);
	glDeleteObjectARB(program_pass1);
	glDeleteObjectARB(program_pass2);
	glDeleteObjectARB(program_pass3);

	log_file << "Renderer shutting down. OpenGL error state: " << gluErrorString(glGetError()) << std::endl;
}

void Renderer::DrawFullScreenQuad(float min_x, float max_x, float min_y, float max_y)
{
	//draw fullscreen quad to trigger fragment processing
	glBegin(GL_TRIANGLE_STRIP);
		glMultiTexCoord2f(GL_TEXTURE0, min_x, min_y);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);

		glMultiTexCoord2f(GL_TEXTURE0, max_x, min_y);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);

		glMultiTexCoord2f(GL_TEXTURE0, min_x, max_y);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);

		glMultiTexCoord2f(GL_TEXTURE0, max_x, max_y);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
	glEnd();
}

void Renderer::DrawFrame(float zoom, float center_x, float center_y, bool is_interactive, int max_iterations, int& iteration)
{
	const float width = (0.6f-(-2.025f))*zoom;
	const float min_x = center_x-(width*0.5f);
	const float max_x = center_x+(width*0.5f);

	const float height = (1.125f-(-1.125f))*zoom;
	const float min_y = center_y-(height*0.5f);
	const float max_y = center_y+(height*0.5f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	//pass 1 - set up initial conditions
	if(iteration==0)
	{
		ping = true;
		glUseProgramObjectARB(program_pass1);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		DrawFullScreenQuad(min_x, max_x, min_y, max_y);
		glUseProgramObjectARB(0);
	}


	//pass 2 - iterate
	while(iteration<=max_iterations)
	{
		glUseProgramObjectARB(program_pass2);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, input_texture[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, input_texture[1]);
		glActiveTexture(GL_TEXTURE0);
		//the output of one pass will be the input into the next
		if(ping)
		{
			glUniform1iARB(uniform_input_p2_location, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
		}
		else
		{
			glUniform1iARB(uniform_input_p2_location, 1);
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		}
		ping = !ping;
		glUniform1fARB(uniform_iteration_location, (GLfloat)iteration);
		DrawFullScreenQuad(min_x, max_x, min_y, max_y);
		++iteration;

		if(!is_interactive) break;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);


	//pass 3 - draw final image
	glUseProgramObjectARB(program_pass3);
	glUniform1iARB(uniform_input_p3_location, 0);
	glUniform1fARB(uniform_max_iterations_location, (GLfloat)max_iterations);
	glUniform1fARB(uniform_iteration_scale_location, (is_interactive?0.1f:1.0f));
	glBindTexture(GL_TEXTURE_2D, input_texture[ping?1:0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		DrawFullScreenQuad(0.0f, 1.0f, 0.0f, 1.0f);
	SDL_GL_SwapBuffers();
}



//helper functions
std::string GetShaderSource(const std::string& filename)
{
	std::ifstream infile(filename.c_str(), std::ios::binary);
	infile.seekg(0, std::ios::end);
	std::streamoff pos = infile.tellg();
	infile.seekg (0, std::ios::beg);
	std::string buffer(pos, ' ');
	infile.read(&buffer[0], pos);
	return buffer;
}

void PrintProgramInfoLog(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);
    if (infologLength > 0)
    {
		infoLog = new char[infologLength];
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		log_file << infoLog << std::endl;
		delete[] infoLog;
    }
}

void PrintShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0)
	{
        infoLog = new char[infologLength];
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		log_file << infoLog << std::endl;
        delete[] infoLog;
    }
}

void LoadShaders(GLenum& program, const std::string& vert_shader_fn, const std::string& frag_shader_fn)
{
	log_file << "===================" << std::endl;
	log_file << "Building Shader" << std::endl;
	log_file << "vertex shader: " << vert_shader_fn << std::endl;
	log_file << "fragment shader: " << frag_shader_fn << std::endl;
	log_file << "Loading source...";
	std::string vertex_shader_source = GetShaderSource(vert_shader_fn);
	std::string fragment_shader_source = GetShaderSource(frag_shader_fn);
	log_file << "done." << std::endl;

	// Create Shader And Program Objects
	program = glCreateProgramObjectARB();
	GLenum vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	GLenum fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	// Load Shader Sources
	char* temp = (char*)vertex_shader_source.c_str();
	glShaderSourceARB(vertex_shader, 1, (const GLcharARB**)&temp, NULL);
	temp = (char*)fragment_shader_source.c_str();
	glShaderSourceARB(fragment_shader, 1, (const GLcharARB**)&temp, NULL);

	// Compile The Shaders
	log_file << "Compiling...";
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);
	log_file << "done." << std::endl;

	// Attach The Shader Objects To The Program Object
	glAttachObjectARB(program, vertex_shader);
	glAttachObjectARB(program, fragment_shader);

	// Link The Program Object
	log_file << "Linking...";
	glLinkProgramARB(program);
	log_file << "done." << std::endl;

	PrintProgramInfoLog(program);
//	PrintShaderInfoLog(program);
	log_file << "===================" << std::endl;
}

void Renderer::Init()
{
	//sanity check, make sure required extension are supported
	log_file << "Checking for presence of required GL extensions...";
	if(!GLEE_ARB_texture_non_power_of_two)
	{
		throw std::exception("ARB_texture_non_power_of_two NOT supported.");
	}
	if(!GLEE_ARB_shader_objects)
	{
		throw std::exception("ARB_shader_objects NOT supported.");
	}
	if(!GLEE_EXT_framebuffer_object)
	{
		throw std::exception("EXT_framebuffer_object NOT supported.");
	}
	log_file << "done." << std::endl;

	/////////////////////////
	//create fbo
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(2, input_texture);

	glBindTexture(GL_TEXTURE_2D, input_texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, input_texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, input_texture[0], 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, input_texture[1], 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	if(status==GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		log_file << "FBO successfully created." << std::endl;
	}
	else
	{
		log_file << "Create FBO failed." << std::endl;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	log_file << "OpenGL error state: " << gluErrorString(glGetError()) << std::endl;
	/////////////////////////

	////////////////////////////////
	//create shaders
	//pass1
	LoadShaders(program_pass1, "mandelbrot.vert", "mandelbrot_pass1.frag");

	//pass 2
	LoadShaders(program_pass2, "mandelbrot.vert", "mandelbrot_pass2.frag");
	glUseProgramObjectARB(program_pass2);
	uniform_iteration_location = glGetUniformLocationARB(program_pass2, "iteration");
	uniform_input_p2_location = glGetUniformLocationARB(program_pass2, "input");
	glUseProgramObjectARB(0);

	//pass 3
	LoadShaders(program_pass3, "mandelbrot.vert", "mandelbrot_pass3.frag");
	glUseProgramObjectARB(program_pass3);
	uniform_max_iterations_location = glGetUniformLocationARB(program_pass3, "max_iterations");
	uniform_input_p3_location = glGetUniformLocationARB(program_pass3, "input");
	uniform_iteration_scale_location = glGetUniformLocationARB(program_pass3, "iteration_scale");
	glUseProgramObjectARB(0);

	log_file << "OpenGL error state: " << gluErrorString(glGetError()) << std::endl;
	//////////////////////////////////////
}