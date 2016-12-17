#include <Windows.h>
#include <myo/myo.hpp>

class MouseEvent{
public:
	MouseEvent()
	{
		isScrolling = false;
		leftDown = false;
		rightDown = false;
		zoomState = 0;  //0未放大; 1,2,3放大
		rotateStep = 1;  //控制参数
		rotateScalar = 1; //控制参数
	}
	POINT point; //鼠标位置
	bool isEnabled;
	bool isScrolling;
	bool leftDown;
	bool rightDown;
	int zoomState;
	int rotateStep;
	int rotateScalar;

	void SavePos()
	{
		::GetCursorPos(&this->point);
	}
	void moveMouse(float dx, float dy);
	void MouseLeftDown();//鼠标左键按下
	void MouseLeftUp(); //鼠标左键放开  
	void MouseLeft();  //点击鼠标左键
	void MouseRightDown(); //鼠标右键按下  
	void MouseRightUp(); //鼠标右键放开  
	void MouseRight();    //点击鼠标右键
	void MouseWheelZoomIn(int scalar);        //ctrl+mousewheel 放大
	void MouseWheelZoomOut(int scalar);     //ctrl+mousewheel 缩小
};