#define _USE_MATH_DEFINES

#include "RotationCapture.hpp"
#include <vector>
#include <cmath>


void RotationCapture::onRotation(const myo::Quaternion<float>& quat)
{
	using std::atan2;
	using std::asin;
	using std::sqrt;
	using std::max;
	using std::min;

	// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
	float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
		1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
	float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
	float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
		1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

	if (roll_staff_vector.size() >= window_length) //full
	{ 
		float roll_stdev, pitch_stdev, yaw_stdev;    //calculate stdev
		stdevCalculate(roll_stdev, pitch_stdev, yaw_stdev);

		float roll_max_element, pitch_max_element, yaw_max_element;  //calculate max element
		maxCalculate(roll_max_element, pitch_max_element, yaw_max_element);

		float roll_move, pitch_move, yaw_move;  //calculate move
		moveCalculate(roll_move, pitch_move, yaw_move);
		
		//judge move direction
		//if roll_move is bigger than pitch_move or yaw_moves
		//set a gate for roll_move. if roll_move is smaller than roll_move_gate, we don't rotate.
		if (roll_stdev > roll_stdev_gate && roll_stdev > pitch_stdev && roll_stdev > yaw_stdev)
		{
			rotate_flag = true;
			roll_move > 0 ? rotate_direction = false : rotate_direction = true;
			rotate_scalar = (int)(roll_max_element * 10);
		}
		else
		{
			rotate_flag = false;
			rotate_direction = false;
			rotate_scalar = 0;
		}
		if (pitch_stdev > pitch_stdev_gate && pitch_stdev > roll_stdev && pitch_stdev > yaw_stdev)
		{
			down_move_flag = true;
		}
		else
		{
			down_move_flag = false;
		}
		slideWindows();  //slide windows
		roll_staff_vector.push_back(roll);  //push
		pitch_staff_vector.push_back(pitch);
		yaw_staff_vector.push_back(yaw);
	}
	else
	{
		roll_staff_vector.push_back(roll);
		pitch_staff_vector.push_back(pitch);
		yaw_staff_vector.push_back(yaw);
	}
}

int RotationCapture::rotationStatus()  //0:non-rotate; 1:clock rotate; -1:non-clock rotate;
{
	if (!rotate_flag)
		return 0;
	if (rotate_direction)
		return 1;
	else
		return -1;
}
int RotationCapture::rotationScalar()
{
	return rotate_scalar;
}
int RotationCapture::DownMoveStatus()
{
	if (down_move_flag)
		return 1;
	else
		return 0;
}
void RotationCapture::stdevCalculate(float &roll_stdev, float &pitch_stdev, float &yaw_stdev)
{
	float roll_sum = std::accumulate(roll_staff_vector.begin(), roll_staff_vector.end(), 0.0); //stdev roll
	float roll_mean = roll_sum / roll_staff_vector.size();
	float roll_accum = 0.0;
	for (std::vector<float>::iterator it = roll_staff_vector.begin(); it != roll_staff_vector.end(); it++)
	{
		roll_accum += (*it - roll_mean)*(*it - roll_mean);
	}
	roll_stdev = sqrt(roll_accum / (roll_staff_vector.size() - 1));

	float pitch_sum = std::accumulate(pitch_staff_vector.begin(), pitch_staff_vector.end(), 0.0);  //stdev pitch
	float pitch_mean = pitch_sum / pitch_staff_vector.size();
	float pitch_accum = 0.0;
	for (std::vector<float>::iterator it = pitch_staff_vector.begin(); it != pitch_staff_vector.end(); it++)
	{
		pitch_accum += (*it - pitch_mean)*(*it - pitch_mean);
	}
	pitch_stdev = sqrt(pitch_accum / (pitch_staff_vector.size() - 1));

	float yaw_sum = std::accumulate(yaw_staff_vector.begin(), yaw_staff_vector.end(), 0.0);   //stdev yaw
	float yaw_mean = yaw_sum / yaw_staff_vector.size();
	float yaw_accum = 0.0;
	for (std::vector<float>::iterator it = yaw_staff_vector.begin(); it != yaw_staff_vector.end(); it++)
	{
		yaw_accum += (*it - yaw_mean)*(*it - yaw_mean);
	}
	yaw_stdev = sqrt(yaw_accum / (yaw_staff_vector.size() - 1));
}

void RotationCapture::maxCalculate(float &roll_max_element, float &pitch_max_element, float &yaw_max_element)
{
	std::vector<float> roll_staff_vector_ant, pitch_staff_vector_ant, yaw_staff_vector_ant;
	for (std::vector<float>::iterator it = roll_staff_vector.begin(); it != roll_staff_vector.end(); it++)
	{
		roll_staff_vector_ant.push_back(std::abs(*it - *roll_staff_vector.begin()));
	}
	
	for (std::vector<float>::iterator it = pitch_staff_vector.begin(); it != pitch_staff_vector.end(); it++)
	{
		pitch_staff_vector_ant.push_back(std::abs(*it - *pitch_staff_vector.begin()));
	}
	
	for (std::vector<float>::iterator it = yaw_staff_vector.begin(); it != yaw_staff_vector.end(); it++)
	{
		yaw_staff_vector_ant.push_back(std::abs(*it - *yaw_staff_vector.begin()));
	}

	roll_max_element = *std::max_element(roll_staff_vector_ant.begin(), roll_staff_vector_ant.end());
	pitch_max_element = *std::max_element(pitch_staff_vector_ant.begin(), pitch_staff_vector_ant.end());
	yaw_max_element = *std::max_element(yaw_staff_vector_ant.begin(), yaw_staff_vector_ant.end());
}

void RotationCapture::moveCalculate(float &roll_move, float &pitch_move, float &yaw_move)
{
	roll_move = (int)((roll_staff_vector.back() - roll_staff_vector.front()) * 360 / PI * 1000 + 0.5) / 1000.0;
	pitch_move = (int)((pitch_staff_vector.back() - pitch_staff_vector.front()) * 360 / PI * 1000 + 0.5) / 1000.0;
	yaw_move = (int)((yaw_staff_vector.back() - yaw_staff_vector.front()) * 360 / PI * 1000 + 0.5) / 1000.0;
}

void RotationCapture::slideWindows()
{
	std::vector<float> roll_staff_vector_ant, pitch_staff_vector_ant, yaw_staff_vector_ant;
	for (std::vector<float>::iterator it = roll_staff_vector.begin() + roll_staff_vector.size() / 2; it != roll_staff_vector.end(); it++)
	{
		roll_staff_vector_ant.push_back(*it);
	}
	roll_staff_vector = roll_staff_vector_ant;
	for (std::vector<float>::iterator it = pitch_staff_vector.begin() + pitch_staff_vector.size() / 2; it != pitch_staff_vector.end(); it++)
	{
		pitch_staff_vector_ant.push_back(*it);
	}
	pitch_staff_vector = pitch_staff_vector_ant;
	for (std::vector<float>::iterator it = yaw_staff_vector.begin() + yaw_staff_vector.size() / 2; it != yaw_staff_vector.end(); it++)
	{
		yaw_staff_vector_ant.push_back(*it);
	}
	yaw_staff_vector = yaw_staff_vector_ant;
}