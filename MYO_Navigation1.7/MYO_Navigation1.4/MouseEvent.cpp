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

void MouseEvent::MouseLeftDown() //����������  
{
	leftDown = true;
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
}

void MouseEvent::MouseLeftUp() //�������ſ�  
{
	leftDown = false;
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));

}

void MouseEvent::MouseLeft() //������
{
	MouseLeftDown();
	MouseLeftUp();
}

void MouseEvent::MouseRightDown() //����Ҽ�����  
{
	rightDown = true;
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
}

void MouseEvent::MouseRightUp() //����Ҽ��ſ�  
{
	rightDown = false;
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &Input, sizeof(INPUT));
}

void MouseEvent::MouseRight() //����Ҽ�
{
	MouseRightDown();
	MouseRightUp();
}
void MouseEvent::MouseWheelZoomIn(int scalar) //mousewheel �Ŵ�
{
	scalar = scalar / 2;
	SavePos();
	mouse_event(MOUSEEVENTF_WHEEL, point.x, point.y, 120 * scalar, 0);
	isScrolling = true;
	zoomState = zoomState + 1;
}
void MouseEvent::MouseWheelZoomOut(int scalar) //mousewheel ��С
{
	scalar = scalar / 2;
	SavePos();
	mouse_event(MOUSEEVENTF_WHEEL, point.x, point.y, -120 * scalar, 0);
	isScrolling = false;
	zoomState = 0;
}