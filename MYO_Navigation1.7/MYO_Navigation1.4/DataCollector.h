#include <myo/myo.hpp>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "MouseMover.hpp"
#include "RotationCapture.hpp"
#include "MouseEvent.hpp"
#include <time.h>
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

class DataCollector : public myo::DeviceListener {
public:

	DataCollector() : mouse_mover()
	{
		myoStatus = 0;
		pos.X = 0; pos.Y = 4;
		print_index = 10;
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
	}
	
	myo::Pose current_pose;
	float mouse_position_dx, mouse_position_dy;
	int rotation_status;  //0:non-rotate; 1:clock rotate; -1:non-clock rotate;
	int rotation_scalar; // rotate more, rotation_scalar will bigger
	int current_rotation_status;  //0:non-rotate; 1:clock rotate; -1:non-clock rotate;
	int current_rotation_scalar; // rotate more, rotation_scalar will bigger

private:
	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg);
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat);
	void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3< float > &gyro);
	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation, myo::WarmupState warmupState);
	void onPose(myo::Myo *myo, uint64_t timestamp, myo::Pose pose);
	void output();
	void operation(int option);
	void characterCalculate(std::vector<emgRawData> emg_raw_data_vector, emgCharacter &emg_character_instance);
	void slideWindow(std::vector<emgRawData> &emg_raw_data_vector);
	void printToCSV(std::vector<emgCharacter> &emg_character_vector, std::string fname);
	std::string convertFromFloat(float Num);
	std::string convertFromInt(int Num);

	MouseMover mouse_mover;
	RotationCapture rotation_capture;
	MouseEvent mouseEvent;
	int myoStatus; ///0:close; 1:open- normal;
	time_t timestamp_before, timestamp_current;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);  //set handle
	COORD pos;
	int print_index;

	std::vector<emgRawData> emg_raw_data_vector;
	emgCharacter emg_character_template_min, emg_character_template_max;
};