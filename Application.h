//Jeremy Brooks - Copyright 2008.
//Please do not copy or use without permission

#ifndef Application_H_
#define Application_H_

#include "InputHandler.h"

class Renderer;
class KeyboardMouseInput;

class Application:public InputHandler
{
public:
	Application();
	~Application();

	void Start();//startup the main loop

	void Update(float delta_time);

	void KeyUp(int k){}
	void KeyDown(int k){}

	void MouseMove(int x, int y);
	void MouseDown(int x, int y, int btn);
	void MouseUp(int x, int y, int btn);

	void Quit(){ running = false; }

private:

	Renderer* renderer;
	KeyboardMouseInput* input;

	float mouse_pos_x, mouse_pos_y;

	float zoom;

	float center_x, center_y;

	float zoom_direction;
	bool do_center;

	bool running;
};

#endif