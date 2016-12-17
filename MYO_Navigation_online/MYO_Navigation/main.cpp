// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// This sample illustrates how to log EMG and IMU data. EMG streaming is only supported for one Myo at a time, and this entire sample is geared to one armband

#define _USE_MATH_DEFINES
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <time.h>
#include <algorithm>
#include <myo/myo.hpp>
#include <vector>
#include <numeric>
#include "svm-predict-online.h"

#define WINDOWS_LENGTH 64
#define COLS_LENGTH 12

typedef struct emgRawData{
	float emg[8];
	int label;
}emgRawData;
typedef struct emgCharacter{
	float emg_mean[8];
	float emg_stdev[8];
	float emg_max[8];
	int label;
}emgCharacter;

std::string fileDescribe;
class DataCollector : public myo::DeviceListener {
public:
	DataCollector()
	{
		emg_character_template_min.emg_mean[0] = 0.9375;
		emg_character_template_min.emg_mean[1] = 1;
		emg_character_template_min.emg_mean[2] = 1.15625;
		emg_character_template_min.emg_mean[3] = 1.3125;
		emg_character_template_min.emg_mean[4] = 0.890625;
		emg_character_template_min.emg_mean[5] = 0.8125;
		emg_character_template_min.emg_mean[6] = 0.734375;
		emg_character_template_min.emg_mean[7] = 0.734375;
		emg_character_template_min.emg_stdev[0] = 0.755929;
		emg_character_template_min.emg_stdev[1] = 0.872872;
		emg_character_template_min.emg_stdev[2] = 1.04262;
		emg_character_template_min.emg_stdev[3] = 1.16667;
		emg_character_template_min.emg_stdev[4] = 0.6636840000000001;
		emg_character_template_min.emg_stdev[5] = 0.701296;
		emg_character_template_min.emg_stdev[6] = 0.5909370000000001;
		emg_character_template_min.emg_stdev[7] = 0.6544640000000001;
		emg_character_template_min.emg_max[0] = 3;
		emg_character_template_min.emg_max[1] = 3;
		emg_character_template_min.emg_max[2] = 4;
		emg_character_template_min.emg_max[3] = 5;
		emg_character_template_min.emg_max[4] = 2;
		emg_character_template_min.emg_max[5] = 2;
		emg_character_template_min.emg_max[6] = 2;
		emg_character_template_min.emg_max[7] = 2;
		emg_character_template_min.label = 0;

		emg_character_template_max.emg_mean[0] = 53.9688;
		emg_character_template_max.emg_mean[1] = 29.2813;
		emg_character_template_max.emg_mean[2] = 52.2188;
		emg_character_template_max.emg_mean[3] = 38.4063;
		emg_character_template_max.emg_mean[4] = 9.40625;
		emg_character_template_max.emg_mean[5] = 56.7344;
		emg_character_template_max.emg_mean[6] = 41.8281;
		emg_character_template_max.emg_mean[7] = 31.8438;
		emg_character_template_max.emg_stdev[0] = 33.2575;
		emg_character_template_max.emg_stdev[1] = 23.9102;
		emg_character_template_max.emg_stdev[2] = 36.2652;
		emg_character_template_max.emg_stdev[3] = 29.6882;
		emg_character_template_max.emg_stdev[4] = 9.77155;
		emg_character_template_max.emg_stdev[5] = 34.8686;
		emg_character_template_max.emg_stdev[6] = 29.7752;
		emg_character_template_max.emg_stdev[7] = 23.6488;
		emg_character_template_max.emg_max[0] = 128;
		emg_character_template_max.emg_max[1] = 82;
		emg_character_template_max.emg_max[2] = 128;
		emg_character_template_max.emg_max[3] = 127;
		emg_character_template_max.emg_max[4] = 46;
		emg_character_template_max.emg_max[5] = 128;
		emg_character_template_max.emg_max[6] = 128;
		emg_character_template_max.emg_max[7] = 119;
		emg_character_template_max.label = 1;


		//model_predict初始化
		char *model_file_name = "emg-character-train-test.model";
		if ((model_predict = svm_load_model(model_file_name)) == 0)
		{
			fprintf(stderr, "can't open model file %s\n", model_file_name);
			exit(1);
		}
		
		x = (struct svm_node *) malloc(24 * sizeof(struct svm_node));
	}

	// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
	{
		emgRawData emg_raw_data_instance;
		for (int i = 0; i < 8; i++)
		{
			//int x = emg[i];
			//std::cout << x << '\t';
			emg_raw_data_instance.emg[i] = emg[i];
		}
		//std::cout << std :: endl;
		emg_raw_data_instance.label = -1;
		if (emg_raw_data_vector.size() == WINDOWS_LENGTH)
		{
			//计算特征 
			emgCharacter emg_character_instance;
			characterCalculate(emg_raw_data_vector, emg_character_instance);
			for (int z = 0; z < 8; z++) std::cout << emg_character_instance.emg_mean[z] << '\t';
			std::cout << std::endl;

			//特征归一化
			for (int j = 0; j < 8; j++)
			{
				emg_character_instance.emg_mean[j] = (emg_character_instance.emg_mean[j] - emg_character_template_min.emg_mean[j])
					/ (emg_character_template_max.emg_mean[j] - emg_character_template_min.emg_mean[j]);
				//emg_character_instance.emg_mean[j] = (emg_character_instance.emg_mean[j]>1) ? (1) : (emg_character_instance.emg_mean[j]);
				//emg_character_instance.emg_mean[j] = (emg_character_instance.emg_mean[j]<0) ? (0) : (emg_character_instance.emg_mean[j]);
				x[j].index = j + 1; x[j].value = emg_character_instance.emg_mean[j];

				emg_character_instance.emg_stdev[j] = (emg_character_instance.emg_stdev[j] - emg_character_template_min.emg_stdev[j])
					/ (emg_character_template_max.emg_stdev[j] - emg_character_template_min.emg_stdev[j]);
				//emg_character_instance.emg_stdev[j] = (emg_character_instance.emg_stdev[j]>1) ? (1) : (emg_character_instance.emg_stdev[j]);
				//emg_character_instance.emg_stdev[j] = (emg_character_instance.emg_stdev[j]<0) ? (0) : (emg_character_instance.emg_stdev[j]);
				x[j+8].index = j + 8 + 1; x[j + 8].value = emg_character_instance.emg_stdev[j];

				emg_character_instance.emg_max[j] = (emg_character_instance.emg_max[j] - emg_character_template_min.emg_max[j]) 
					/ (emg_character_template_max.emg_max[j] - emg_character_template_min.emg_max[j]);
				//emg_character_instance.emg_max[j] = (emg_character_instance.emg_max[j]>1) ? (1) : (emg_character_instance.emg_max[j]);
				//emg_character_instance.emg_max[j] = (emg_character_instance.emg_max[j]<0) ? (0) : (emg_character_instance.emg_max[j]);
				x[j + 16].index = j + 16 + 1; x[j + 16].value = emg_character_instance.emg_max[j];
			}
			//for (int j = 0; j < 24; j++)
			//{
			//	std::cout << x[j].index << ":" << x[j].value << "  ";
			//}
			//std::cout << std::endl;
			//预测结果
			std::cout << svm_predict(model_predict, x) << std::endl;
			//滑动窗口
			slideWindow(emg_raw_data_vector);
			//清空缓存
			//emg_raw_data_vector.clear();
		}
		emg_raw_data_vector.push_back(emg_raw_data_instance);
	}

	// onOrientationData is called whenever new orientation data is provided
	// Be warned: This will not make any distiction between data from other Myo armbands
	void onOrientationData(myo::Myo *myo, uint64_t timestamp, const myo::Quaternion< float > &rotation) 
	{
		
	}

	// onAccelerometerData is called whenever new acceleromenter data is provided
	// Be warned: This will not make any distiction between data from other Myo armbands
	void onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel) 
	{

	}

	// onGyroscopeData is called whenever new gyroscope data is provided
	// Be warned: This will not make any distiction between data from other Myo armbands
	void onGyroscopeData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &gyro) 
	{

	}

	void onConnect(myo::Myo *myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion) {
		//Reneable streaming
		//myo->setStreamEmg(myo::Myo::streamEmgEnabled);
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
	// The files we are logging to
	std::ofstream emgFile;
	std::ofstream gyroFile;
	std::ofstream orientationFile;
	std::ofstream orientationEulerFile;
	std::ofstream accelerometerFile;

	std::vector<emgRawData> emg_raw_data_vector;
	emgCharacter emg_character_template_min, emg_character_template_max;

	struct svm_model* model_predict;
	struct svm_node *x;
};

int main(int argc, char** argv)
{
	// We catch any exceptions that might occur below -- see the catch statement for more details.
	try {

		// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
		// publishing your application. The Hub provides access to one or more Myos.
		myo::Hub hub("com.undercoveryeti.myo-data-capture");

		std::cout << "Attempting to find a Myo..." << std::endl;

		std::cout << "Input filename:";

		// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
		// immediately.
		// waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
		// if that fails, the function will return a null pointer.
		myo::Myo* myo = hub.waitForMyo(10000);

		// If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}

		// We've found a Myo.
		std::cout << "Connected to a Myo armband! Logging to the file system. Check your home folder or the folder this application lives in." << std::endl << std::endl;

		// Next we enable EMG streaming on the found Myo.
		myo->setStreamEmg(myo::Myo::streamEmgEnabled);

		// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
		DataCollector collector;

		// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		hub.addListener(&collector);

		// Finally we enter our main loop.
		while (1) {
			// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
			// In this case, we wish to update our display 50 times a second, so we run for 1000/20 milliseconds.
			hub.run(1);
		}

		// If a standard exception occurred, we print out its message and exit.
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
}
