#include "DataCollector.h"
#include "svm-predict-online.h"

void DataCollector::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
	std::cout << "onEmgData" << std::endl;
	//emgRawData emg_raw_data_instance;
	//for (int i = 0; i < 8; i++)
	//{
	//	emg_raw_data_instance.emg[i] = emg[i];
	//}
	//emg_raw_data_instance.label = -1;
	//if (emg_raw_data_vector.size() == WINDOWS_LENGTH)
	//{
	//	//计算特征 
	//	emgCharacter emg_character_instance;
	//	characterCalculate(emg_raw_data_vector, emg_character_instance);
	//	//特征归一化
	//	for (int j = 0; j < 8; j++)
	//	{
	//		emg_character_instance.emg_max[j] = (emg_character_instance.emg_max[j] - emg_character_template_min.emg_max[j]) 
	//			/ (emg_character_template_max.emg_max[j] - emg_character_template_min.emg_max[j]);
	//		emg_character_instance.emg_max[j] = (emg_character_instance.emg_max[j]>1) ? (1) : (emg_character_instance.emg_max[j]);
	//		emg_character_instance.emg_max[j] = (emg_character_instance.emg_max[j]<0) ? (0) : (emg_character_instance.emg_max[j]);

	//		emg_character_instance.emg_mean[j] = (emg_character_instance.emg_mean[j] - emg_character_template_min.emg_mean[j])
	//			/ (emg_character_template_max.emg_mean[j] - emg_character_template_min.emg_mean[j]);
	//		emg_character_instance.emg_mean[j] = (emg_character_instance.emg_mean[j]>1) ? (1) : (emg_character_instance.emg_mean[j]);
	//		emg_character_instance.emg_mean[j] = (emg_character_instance.emg_mean[j]<0) ? (0) : (emg_character_instance.emg_mean[j]);

	//		emg_character_instance.emg_stdev[j] = (emg_character_instance.emg_stdev[j] - emg_character_template_min.emg_stdev[j])
	//			/ (emg_character_template_max.emg_stdev[j] - emg_character_template_min.emg_stdev[j]);
	//		emg_character_instance.emg_stdev[j] = (emg_character_instance.emg_stdev[j]>1) ? (1) : (emg_character_instance.emg_stdev[j]);
	//		emg_character_instance.emg_stdev[j] = (emg_character_instance.emg_stdev[j]<0) ? (0) : (emg_character_instance.emg_stdev[j]);
	//	}
	//	//预测结果
	//	predict_online();
	//	//滑动窗口
	//	slideWindow(emg_raw_data_vector);
	//	//清空缓存
	//	emg_raw_data_vector.clear();
	//}
	//std::cout << "111" << std::endl;
	//emg_raw_data_vector.push_back(emg_raw_data_instance);
}

void DataCollector::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
{
	mouse_mover.onOrientation(quat);
	rotation_capture.onRotation(quat);
}

void DataCollector::onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation, myo::WarmupState warmupState)
{
	/*mouse_mover.setXTowardsWrist(xDirection == myo::xDirectionTowardWrist);*/
}

void DataCollector::onPose(myo::Myo *myo, uint64_t timestamp, myo::Pose pose)
{
	current_pose = pose;
	operation(1);
}

void DataCollector::onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3< float > &gyro)
{
	mouse_mover.onGyroscope(gyro);
	mouse_position_dx = mouse_mover.dx();
	mouse_position_dy = mouse_mover.dy();
	mouseEvent.moveMouse(mouse_position_dx, mouse_position_dy);

	timestamp_current = std::time(0);

	//rotation status
	rotation_status = rotation_capture.rotationStatus();
	rotation_scalar = rotation_capture.rotationScalar();
	if (rotation_status != 0)
	{
		current_rotation_scalar = rotation_scalar;
		current_rotation_status = rotation_status;
	}

	//output
	if (print_index == 10)
	{
		if (rotation_status != 0 && current_pose.type() == myo::Pose::fingersSpread)
		{
			if (rotation_status > 0)
			{
				mouseEvent.MouseWheelZoomIn(rotation_scalar);
			}
			if (rotation_status < 0)
			{
				mouseEvent.MouseWheelZoomOut(rotation_scalar);
			}
		}
		output();
		print_index = 0;
	}
	print_index++;	
}

void DataCollector::output()
{
	SetConsoleCursorPosition(hOut, pos);

	std::cout << "当前手势：" << std::setw(20) << current_pose.toString() << std::endl;
	std::cout << "移动幅度：" << std::setw(4) << mouse_position_dx << std::setw(4) << mouse_position_dy << std::endl;
	std::cout << "旋转状态：" << std::setw(4) << rotation_status << std::endl;
	std::cout << "旋转尺度：" << std::setw(4) << rotation_scalar << std::endl;
	std::cout << "先前旋转状态：" << std::setw(4) << current_rotation_status << std::endl;
	std::cout << "先前旋转尺度：" << std::setw(4) << current_rotation_scalar << std::endl;
	std::cout << "MYO 状态：" << std::setw(4) << myoStatus << std::endl;
	std::cout << "MYO 放大：" << std::setw(4) << mouseEvent.isScrolling << std::endl;
	std::cout << std::setw(20) << timestamp_current - timestamp_before << std::endl;
}

void DataCollector::operation(int option)
{
	if (timestamp_current - timestamp_before <= 3)
	{
		return;
	}
	timestamp_before = timestamp_current;
	if (option == 1)      //调用控制程序者，来自onPose
	{
		if (myoStatus == 0 && current_pose.type() != myo::Pose::doubleTap) //处于关闭状态，直接退出控制
			return;
		mouseEvent.MouseLeftUp(); //cancle left down
		if (current_pose.type() == myo::Pose::doubleTap)
		{
			myoStatus = (myoStatus == 0) ? 1 : 0;
			return;
		}
		else if (current_pose.type() == myo::Pose::fist)
		{
			mouseEvent.MouseLeftDown();
		}
		else if (current_pose.type() == myo::Pose::waveIn)
		{
			if (current_rotation_status == 1)
			{
				mouseEvent.MouseWheelZoomIn(current_rotation_scalar);
			}
			else if (current_rotation_status == -1)
			{
				mouseEvent.MouseWheelZoomOut(current_rotation_scalar);
			}
		}
		else if (current_pose.type() == myo::Pose::fingersSpread)
		{
			//mouseEvent.MouseRight();
		}
	}
}

void DataCollector::characterCalculate(std::vector<emgRawData> emg_raw_data_vector, emgCharacter &emg_character_instance)
{
	std::vector<float> emg1, emg2, emg3, emg4, emg5, emg6, emg7, emg8;  //the absolute emg data
	std::vector<float> emg1_a, emg2_a, emg3_a, emg4_a, emg5_a, emg6_a, emg7_a, emg8_a;  //the original emg data
	int index = 0;
	for (std::vector<emgRawData>::iterator emg_raw_data_vector_itr = emg_raw_data_vector.begin(); emg_raw_data_vector_itr != emg_raw_data_vector.end(); emg_raw_data_vector_itr++)
	{
		emg1.push_back(std::abs(emg_raw_data_vector_itr->emg[0]));
		emg2.push_back(std::abs(emg_raw_data_vector_itr->emg[1]));
		emg3.push_back(std::abs(emg_raw_data_vector_itr->emg[2]));
		emg4.push_back(std::abs(emg_raw_data_vector_itr->emg[3]));
		emg5.push_back(std::abs(emg_raw_data_vector_itr->emg[4]));
		emg6.push_back(std::abs(emg_raw_data_vector_itr->emg[5]));
		emg7.push_back(std::abs(emg_raw_data_vector_itr->emg[6]));
		emg8.push_back(std::abs(emg_raw_data_vector_itr->emg[7]));

		emg1_a.push_back((emg_raw_data_vector_itr->emg[0]));
		emg2_a.push_back((emg_raw_data_vector_itr->emg[1]));
		emg3_a.push_back((emg_raw_data_vector_itr->emg[2]));
		emg4_a.push_back((emg_raw_data_vector_itr->emg[3]));
		emg5_a.push_back((emg_raw_data_vector_itr->emg[4]));
		emg6_a.push_back((emg_raw_data_vector_itr->emg[5]));
		emg7_a.push_back((emg_raw_data_vector_itr->emg[6]));
		emg8_a.push_back((emg_raw_data_vector_itr->emg[7]));

		index++;
	}
	//mean
	emg_character_instance.emg_mean[0] = std::accumulate(emg1.begin(), emg1.end(), 0.0) / emg1.size();
	emg_character_instance.emg_mean[1] = std::accumulate(emg2.begin(), emg2.end(), 0.0) / emg2.size();
	emg_character_instance.emg_mean[2] = std::accumulate(emg3.begin(), emg3.end(), 0.0) / emg3.size();
	emg_character_instance.emg_mean[3] = std::accumulate(emg4.begin(), emg4.end(), 0.0) / emg4.size();
	emg_character_instance.emg_mean[4] = std::accumulate(emg5.begin(), emg5.end(), 0.0) / emg5.size();
	emg_character_instance.emg_mean[5] = std::accumulate(emg6.begin(), emg6.end(), 0.0) / emg6.size();
	emg_character_instance.emg_mean[6] = std::accumulate(emg7.begin(), emg7.end(), 0.0) / emg7.size();
	emg_character_instance.emg_mean[7] = std::accumulate(emg8.begin(), emg8.end(), 0.0) / emg8.size();
	
	//stdev
	float emg1_accum = 0.0; //emg1_accum, emg1, emg_character_instance.emg_mean[0], emg_character_instance.emg_stdev[0]
	for (std::vector<float>::iterator it = emg1.begin(); it != emg1.end(); it++)
	{
		emg1_accum += (*it - emg_character_instance.emg_mean[0])*(*it - emg_character_instance.emg_mean[0]);
	}
	emg_character_instance.emg_stdev[0] = sqrt(emg1_accum / (emg1.size() - 1));

	float emg2_accum = 0.0; //emg2_accum, emg2, emg_character_instance.emg_mean[1], emg_character_instance.emg_stdev[1]
	for (std::vector<float>::iterator it = emg2.begin(); it != emg2.end(); it++)
	{
		emg2_accum += (*it - emg_character_instance.emg_mean[1])*(*it - emg_character_instance.emg_mean[1]);
	}
	emg_character_instance.emg_stdev[1] = sqrt(emg2_accum / (emg2.size() - 1));

	float emg3_accum = 0.0; //emg3_accum, emg3, emg_character_instance.emg_mean[2], emg_character_instance.emg_stdev[2]
	for (std::vector<float>::iterator it = emg3.begin(); it != emg3.end(); it++)
	{
		emg3_accum += (*it - emg_character_instance.emg_mean[2])*(*it - emg_character_instance.emg_mean[2]);
	}
	emg_character_instance.emg_stdev[2] = sqrt(emg3_accum / (emg3.size() - 1));

	float emg4_accum = 0.0; //emg4_accum, emg4, emg_character_instance.emg_mean[3], emg_character_instance.emg_stdev[3]
	for (std::vector<float>::iterator it = emg4.begin(); it != emg4.end(); it++)
	{
		emg4_accum += (*it - emg_character_instance.emg_mean[3])*(*it - emg_character_instance.emg_mean[3]);
	}
	emg_character_instance.emg_stdev[3] = sqrt(emg4_accum / (emg4.size() - 1));

	float emg5_accum = 0.0; //emg5_accum, emg5, emg_character_instance.emg_mean[4], emg_character_instance.emg_stdev[4]
	for (std::vector<float>::iterator it = emg5.begin(); it != emg5.end(); it++)
	{
		emg5_accum += (*it - emg_character_instance.emg_mean[4])*(*it - emg_character_instance.emg_mean[4]);
	}
	emg_character_instance.emg_stdev[4] = sqrt(emg5_accum / (emg5.size() - 1));

	float emg6_accum = 0.0; //emg6_accum, emg6, emg_character_instance.emg_mean[5], emg_character_instance.emg_stdev[5]
	for (std::vector<float>::iterator it = emg6.begin(); it != emg6.end(); it++)
	{
		emg6_accum += (*it - emg_character_instance.emg_mean[5])*(*it - emg_character_instance.emg_mean[5]);
	}
	emg_character_instance.emg_stdev[5] = sqrt(emg6_accum / (emg6.size() - 1));

	float emg7_accum = 0.0; //emg7_accum, emg7, emg_character_instance.emg_mean[6], emg_character_instance.emg_stdev[6]
	for (std::vector<float>::iterator it = emg7.begin(); it != emg7.end(); it++)
	{
		emg7_accum += (*it - emg_character_instance.emg_mean[6])*(*it - emg_character_instance.emg_mean[6]);
	}
	emg_character_instance.emg_stdev[6] = sqrt(emg7_accum / (emg7.size() - 1));

	float emg8_accum = 0.0; //emg8_accum, emg8, emg_character_instance.emg_mean[7], emg_character_instance.emg_stdev[7]
	for (std::vector<float>::iterator it = emg8.begin(); it != emg8.end(); it++)
	{
		emg8_accum += (*it - emg_character_instance.emg_mean[7])*(*it - emg_character_instance.emg_mean[7]);
	}
	emg_character_instance.emg_stdev[7] = sqrt(emg8_accum / (emg8.size() - 1));
	//max
	emg_character_instance.emg_max[0] = *std::max_element(emg1.begin(), emg1.end());
	emg_character_instance.emg_max[1] = *std::max_element(emg2.begin(), emg2.end());
	emg_character_instance.emg_max[2] = *std::max_element(emg3.begin(), emg3.end());
	emg_character_instance.emg_max[3] = *std::max_element(emg4.begin(), emg4.end());
	emg_character_instance.emg_max[4] = *std::max_element(emg5.begin(), emg5.end());
	emg_character_instance.emg_max[5] = *std::max_element(emg6.begin(), emg6.end());
	emg_character_instance.emg_max[6] = *std::max_element(emg7.begin(), emg7.end());
	emg_character_instance.emg_max[7] = *std::max_element(emg8.begin(), emg8.end());
	
	//label
	emg_character_instance.label = emg_raw_data_vector.begin()->label;
}

//slide windows
void DataCollector::slideWindow(std::vector<emgRawData> &emg_raw_data_vector)
{
	int emg_raw_data_vector_size = emg_raw_data_vector.size();
	emg_raw_data_vector.erase(emg_raw_data_vector.begin(), emg_raw_data_vector.begin() + emg_raw_data_vector_size / 2);
}

std::string DataCollector::convertFromInt(int Num)
{
	std::ostringstream oss;
	oss << Num;
	std::string str(oss.str());
	return str;
}

std::string DataCollector::convertFromFloat(float Num)
{
	std::ostringstream oss;
	oss << Num;
	std::string str(oss.str());
	return str;
}

//write file
void DataCollector::printToCSV(std::vector<emgCharacter> &emg_character_vector, std::string fname)
{
	//std::ofstream fout_file_name(fname, std::ios::app);
	std::ofstream fout_file_name(fname, std::ios::out);
	if (!fout_file_name.is_open())
	{
		std::cout << "Can't open file!" << std::endl;
		return;
	}
	std::string str;
	for (std::vector<emgCharacter>::iterator it = emg_character_vector.begin(); it != emg_character_vector.end(); it++)
	{
		str = "";
		str.append(convertFromInt(it->label));
		str.append(" ");
		int index = 0;
		for (int i = 0; i < 8; i++)
		{
			index++;
			str.append(convertFromInt(index));
			str.append(":");
			str.append(convertFromFloat(it->emg_mean[i]));
			str.append(" ");
		}
		for (int i = 0; i < 8; i++)
		{
			index++;
			str.append(convertFromInt(index));
			str.append(":");
			str.append(convertFromFloat(it->emg_stdev[i]));
			str.append(" ");
		}
		for (int i = 0; i < 8; i++)
		{
			index++;
			str.append(convertFromInt(index));
			str.append(":");
			str.append(convertFromFloat(it->emg_max[i]));
			str.append(" ");
		}
		str.append("\n");
		//std::cout << str;
		fout_file_name << str;
	}
	emg_character_vector.clear();
	fout_file_name.close();
}
