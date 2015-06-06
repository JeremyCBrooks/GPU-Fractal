//Jeremy Brooks - Copyright 2008.
//Please do not copy or use without permission

#include <iostream>
#include <sstream>
#include <fstream>

#include "Application.h"
#include "Renderer.h"
#include "KeyboardMouseInput.h"

extern std::ofstream log_file;//external log file declared in main

Application::Application(void):running(false)
{
	log_file << "===================" << std::endl;
	//initialize SDL
	if(-1==SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE))
	{
		throw std::exception("SDL_Init failed.");
	}

	renderer = new Renderer();
	input = new KeyboardMouseInput();

	log_file << "===================" << std::endl;
}

Application::~Application()
{
	log_file << "===================" << std::endl;
	log_file << "Application shutting down..." << std::endl;

	delete renderer;
	delete input;

	//shutdown SDL
	log_file << "Shutting down SDL...";
	SDL_Quit();
	log_file << "done." << std::endl;

	log_file << "Application shut down complete." << std::endl;
	log_file << "===================" << std::endl;
}

void Application::Update(float delta_time)
{
	if(do_center)
	{
		const float width = (0.6f-(-2.025f))*zoom;
		float min_x = center_x-width*0.5f;
		float new_center_x = ((mouse_pos_x/float(WINDOW_WIDTH-1) * width) + min_x);

		const float height = (1.125f-(-1.125f))*zoom;
		float min_y = center_y-height*0.5f;
		float new_center_y = (((1.0f-(mouse_pos_y/float(WINDOW_HEIGHT-1))) * height) + min_y);

		center_x += (new_center_x-center_x)*delta_time;
		center_y += (new_center_y-center_y)*delta_time;
	}

	zoom*=zoom_direction;
}

void Application::Start(void)
{
	zoom = 1.0f;
	center_x = -0.7125f;
	center_y = 0.0f;
	zoom_direction = 1.0f;

	do_center = false;

	float delta_frame_time = 0.0f;

	float time_since_last_update = 0.0f;
	running = true;
	unsigned int last_time = SDL_GetTicks();

	int iteration = 0;
	while(running)
	{
		//get mouse and keyboard input
		input->HandleInput(*this);

		//compute window zoom and center based on mouse input
		Update(delta_frame_time);

		//if the user is interacting with the screen (zooming or panning), use less 
		//iterations to try and keep the frame rate up.
		bool is_interactive = false;
		int max_iterations = 2000;

		if(do_center || zoom_direction>1.0f || zoom_direction<1.0f)
		{
			iteration = 0;//reset
			is_interactive = true;
			max_iterations = 200;
		}

		renderer->DrawFrame(zoom, center_x, center_y, is_interactive, max_iterations, iteration);

		time_since_last_update+=delta_frame_time;

		unsigned int this_time = SDL_GetTicks();
		delta_frame_time = (float)(this_time-last_time)/1000.0f;
		last_time = this_time;

		std::stringstream title;
		title << "FPS: " << 1.0f/delta_frame_time;
		SDL_WM_SetCaption(title.str().c_str(), NULL);
	}
}

void Application::MouseMove(int x, int y)
{
	mouse_pos_x = float(x);
	mouse_pos_y = float(y);
}

void Application::MouseUp(int x, int y, int mouse_button)
{
	if(mouse_button==SDL_BUTTON_LEFT || mouse_button==SDL_BUTTON_RIGHT) zoom_direction = 1.0f;
	if(mouse_button==SDL_BUTTON_MIDDLE) do_center = false;
}

void Application::MouseDown(int x, int y, int mouse_button)
{
	mouse_pos_x = float(x);
	mouse_pos_y = float(y);
	if(mouse_button==SDL_BUTTON_LEFT) zoom_direction = 0.95f;
	if(mouse_button==SDL_BUTTON_RIGHT) zoom_direction = 1.05f;
	if(mouse_button==SDL_BUTTON_MIDDLE) do_center = true;
}
