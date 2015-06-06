//Jeremy Brooks - Copyright 2008.
//Please do not copy or use without permission

#include <SDL.h>

#include "KeyboardMouseInput.h"

#include <iostream>
#include <fstream>

extern std::ofstream log_file;//external log file declared in main

KeyboardMouseInput::KeyboardMouseInput()
{
	joystick = SDL_JoystickOpen(0);
}

KeyboardMouseInput::~KeyboardMouseInput()
{
	log_file << "KeyboardMouseInput shutting down. Flushing event queue...";
	//drain the message queue
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
	}
	SDL_JoystickClose(joystick);
	log_file << "done." << std::endl;
}

void KeyboardMouseInput::HandleInput(InputHandler& handler)
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				handler.Quit();
				break;
			}

			//handle key events
			case SDL_KEYUP:
			{
				SDLKey key = event.key.keysym.sym;
#ifdef _DEBUG
				std::cout << "Key code: " << key << std::endl;
#endif
				if(key==27)//esc
				{
					handler.Quit();
				}
				else
				{
					handler.KeyUp(key);
				}
				break;
			}

			case SDL_KEYDOWN:
			{
				SDLKey key = event.key.keysym.sym;
#ifdef _DEBUG
				std::cout << "Key code: " << key << std::endl;
#endif
				handler.KeyDown(key);

				break;
			}

			//handle mouse events
			case SDL_MOUSEMOTION:
			{
				handler.MouseMove(event.motion.x, event.motion.y);
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				Uint8 mouse_button = event.button.button;
				handler.MouseDown(event.button.x, event.button.y, mouse_button);

				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				Uint8 mouse_button = event.button.button;
				handler.MouseUp(event.button.x, event.button.y, mouse_button);

				break;
			}
		}
	}
}
