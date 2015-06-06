//Jeremy Brooks - Copyright 2008.
//Please do not copy or use without permission

#ifndef KEYBOARDMOUSEINPUT_H_
#define KEYBOURDMOUSEINPUT_H_

#include "InputHandler.h"

class KeyboardMouseInput
{
public:
	KeyboardMouseInput();
	~KeyboardMouseInput();

	void HandleInput(InputHandler&);

private:

	SDL_Joystick* joystick;
};

#endif