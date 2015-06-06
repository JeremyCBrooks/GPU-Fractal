//Jeremy Brooks - Copyright 2008.
//Please do not copy or use without permission

#ifndef _INPUT_HANDLER_H_
#define _INPUT_HANDLER_H_

class InputHandler
{
public:
	virtual ~InputHandler(){}

	virtual	void Quit()=0;

	virtual	void KeyUp(int k)=0;
	virtual	void KeyDown(int k)=0;

	virtual	void MouseMove(int x, int y)=0;
	virtual void MouseDown(int x, int y, int btn)=0;
	virtual void MouseUp(int x, int y, int btn)=0;
};

#endif