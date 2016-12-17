#include <Windows.h>
#include <myo/myo.hpp>

class MouseEvent{
public:
	MouseEvent()
	{
		isScrolling = false;
		leftDown = false;
		rightDown = false;
		zoomState = 0;  //0δ�Ŵ�; 1,2,3�Ŵ�
		rotateStep = 1;  //���Ʋ���
		rotateScalar = 1; //���Ʋ���
	}
	POINT point; //���λ��
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
	void MouseLeftDown();//����������
	void MouseLeftUp(); //�������ſ�  
	void MouseLeft();  //���������
	void MouseRightDown(); //����Ҽ�����  
	void MouseRightUp(); //����Ҽ��ſ�  
	void MouseRight();    //�������Ҽ�
	void MouseWheelZoomIn(int scalar);        //ctrl+mousewheel �Ŵ�
	void MouseWheelZoomOut(int scalar);     //ctrl+mousewheel ��С
};