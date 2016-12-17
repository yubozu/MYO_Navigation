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
				std::cout << "执行操作：关    闭MYO；  当前MYO状态：关闭-         " << std::endl;
				break;
		case 1:
			if (variable_before == 0)
			{
				std::cout << "执行操作：开    启MYO；  当前MYO状态：开启- 正常状态" << std::endl;
			}
			if (variable_before == 4)
			{
				std::cout << "执行操作：左下复原MYO；  当前MYO状态：开启- 正常状态" << std::endl;
				mouseEvent.MouseLeftUp();
			}
			break;
		case 2:
				std::cout << "执行操作：左    击MYO；  当前MYO状态：开启- 正常状态" << std::endl;
				mouseEvent.MouseLeft();
			break;
		case 4:
				std::cout << "执行操作：左键按下MYO；  当前MYO状态：开启- 左下状态" << std::endl;
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