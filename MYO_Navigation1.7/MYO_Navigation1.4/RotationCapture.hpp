#pragma once
#define PI 3.1415926
#define WINDOW_LENGTH 24
#define ROLL_MOVE_GATE_UNIT 0.0035
#define ROLL_STDEV_GATE_UNIT 0.0042
#define PITCH_MOVE_GATE_UNIT 0.0035
#define PITCH_STDEV_GATE_UNIT 0.0042
#include <vector>
#include <myo/myo.hpp>
#include <windows.h>
#include <iomanip>
#include <algorithm>
#include <numeric>

class RotationCapture {
public:
	RotationCapture()
	{
		window_length = (WINDOW_LENGTH);
		roll_move_gate = (ROLL_MOVE_GATE_UNIT)* window_length;
		roll_stdev_gate = (ROLL_STDEV_GATE_UNIT)* window_length;
		pitch_move_gate = (PITCH_MOVE_GATE_UNIT)* window_length;
		pitch_stdev_gate = (PITCH_STDEV_GATE_UNIT)* window_length;
		roll_staff_vector.clear();
		pitch_staff_vector.clear();
		yaw_staff_vector.clear();

		rotate_direction = false;
		rotate_flag = false;
		down_move_flag = false;
	}
	void onRotation(const myo::Quaternion<float>& quat);
	int rotationStatus();
	int rotationScalar();
	int DownMoveStatus();

private:
	void stdevCalculate(float &a, float &b, float &c); //calculate stdev character
	void maxCalculate(float &roll_max_element, float &pitch_max_element, float &yaw_max_element); //calculate max element character
	void moveCalculate(float &roll_move, float &pitch_move, float &yaw_move);  //calculate move character
	void slideWindows();  //slide windows

	int window_length;
	std::vector<float> roll_staff_vector, pitch_staff_vector, yaw_staff_vector;
	float roll_move_gate, roll_stdev_gate;
	float pitch_move_gate, pitch_stdev_gate;
	bool rotate_flag;  //true:rotate; flase:non-rotate
	bool rotate_direction; //true:clock;flase:non-clock;
	int rotate_scalar;
	bool down_move_flag;  //true:down_move; flase:non-down_move
};
