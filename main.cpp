//Jeremy Brooks - Copyright 2008.
//Please do not copy or use without permission

//interactive Mandelbrot fractal computed on the GPU.
//left mouse button - zoom in
//right mouse button - zoom out
//middle mouse drag - pan 

#include <fstream>
#include <ctime>
#include <SDL.h>

#include "Application.h"

//this log file is shared across the other source files via extern
std::ofstream log_file("application.log");

//program entry point
int SDL_main(int argc, char **argv)
{
	try
	{
		log_file << "Begin setup..." << std::endl;

		//set up application and all subsystems, load resources, get this thing up and running!
		{
			Application app;

			log_file << "Setup complete." << std::endl;

			//start main loop, does not return until game exits
			app.Start();
		}

		log_file << "Clean exit.";
	}
	catch(const std::exception& ex)
	{
		log_file << "Application terminated unexpectedly.  " << ex.what() << std::endl;
	}

	//that's all folks
	return 0;
}