#ifndef VIDEOWINDOWOVERLAY_H
#define VIDEOWINDOWOVERLAY_H

#include <opencv2/opencv.hpp>

#include "RobotData.h"

#define SERVO_PROMPT_STRING		"SERVO: %1"
#define ACC_XYZ_PROMPT_STRING	"ACC_%1: %2"
#define GYRO_XYZ_PROMPT_STRING	"GYRO_%1: %2"
#define PID_PIDU_PROMPT_STRING	"PID_%1: %2"
#define GYRO_Z_SP_PROMPT_STRING	"GZSP: %1"
#define MOTOR_LR_PROMPT_STRING	"THR_%1: %2"

class VideoWindowOverlay
{
public:
	VideoWindowOverlay();

    void drawRobotData(cv::Mat& frame, const RobotData& robotData);
	void toggleForeground(cv::Mat& frame, const RobotData& robotData);

private:
	inline void drawImuData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawPidData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawServoData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawMotorData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);

	double _fontScale;
	int _doublePrecision;

	int _marginW;
	int _marginH;
	int _dX;
	int _dY;
	cv::Scalar _foreground;

	
};

#endif
