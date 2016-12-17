#include "MouseEvent.hpp"

void MouseEvent::moveMouse(float dx, float dy) // Windows mouse movement code
{ 
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;

	input.mi.dx = (LONG)dx;
	input.mi.dy = (LONG)dy;

	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseEvent::MouseLeftDown() //鼠标左键按下  
{
	leftDown = true;
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
}

void MouseEvent::MouseLeftUp() //鼠标左键放开  
{
	leftDown = false;
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));

}

void MouseEvent::MouseLeft() //点击左键
{
	MouseLeftDown();
	MouseLeftUp();
}

void MouseEvent::MouseRightDown() //鼠标右键按下  
{
	rightDown = true;
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
}

void MouseEvent::MouseRightUp() //鼠标右键放开  
{
	rightDown = false;
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &Input, sizeof(INPUT));
}

void MouseEvent::MouseRight() //点击右键
{
	MouseRightDown();
	MouseRightUp();
}
void MouseEvent::MouseWheelZoomIn(int scalar) //mousewheel 放大
{
	scalar = scalar / 2;
	SavePos();
	mouse_event(MOUSEEVENTF_WHEEL, point.x, point.y, 120 * scalar, 0);
	isScrolling = true;
	zoomState = zoomState + 1;
}
void MouseEvent::MouseWheelZoomOut(int scalar) //mousewheel 缩小
{
	scalar = scalar / 2;
	SavePos();
	mouse_event(MOUSEEVENTF_WHEEL, point.x, point.y, -120 * scalar, 0);
	isScrolling = false;
	zoomState = 0;
}