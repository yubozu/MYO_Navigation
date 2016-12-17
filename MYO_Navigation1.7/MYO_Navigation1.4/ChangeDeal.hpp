#ifndef CHANGE_DEAL
#define CHANGE_DEAL
#include <iostream>
#include <iomanip>
#include "MouseEvent.hpp"

class ChangeDeal{
public:
	ChangeDeal()
	{
		variable_before = -1;
	}
	void onChangeDeal(int variable_current, float mouse_position_dx, float mouse_position_dy)
	{
		mouseEvent.moveMouse(mouse_position_dx, mouse_position_dy);
		if (variable_before == variable_current)
			return;

		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);  //set handle
		COORD pos;
		pos.X = 0; pos.Y = 9;
		SetConsoleCursorPosition(hOut, pos);

		switch (variable_current)
		{
		default:
		case 0:
				std::cout << "ִ�в�������    ��MYO��  ��ǰMYO״̬���ر�-         " << std::endl;
				break;
		case 1:
			if (variable_before == 0)
			{
				std::cout << "ִ�в�������    ��MYO��  ��ǰMYO״̬������- ����״̬" << std::endl;
			}
			if (variable_before == 4)
			{
				std::cout << "ִ�в��������¸�ԭMYO��  ��ǰMYO״̬������- ����״̬" << std::endl;
				mouseEvent.MouseLeftUp();
			}
			break;
		case 2:
				std::cout << "ִ�в�������    ��MYO��  ��ǰMYO״̬������- ����״̬" << std::endl;
				mouseEvent.MouseLeft();
			break;
		case 4:
				std::cout << "ִ�в������������MYO��  ��ǰMYO״̬������- ����״̬" << std::endl;
				mouseEvent.MouseLeftDown();
			break;
		}
		variable_before = variable_current;
	}
private:
	int variable_before;
	MouseEvent mouseEvent;
};

#endif