#ifndef VIDEOWINDOWOVERLAY_H
#define VIDEOWINDOWOVERLAY_H

#include <opencv2/opencv.hpp>

#include "RobotData.h"
#include "VideoProcessing.h"
#include "DigitalZoom.h"

#define SERVO_PROMPT_STRING		"SERVO: %1"
#define ACC_XYZ_PROMPT_STRING	"ACC_%1: %2"
#define GYRO_XYZ_PROMPT_STRING	"GYRO_%1: %2"
#define PID_PIDU_PROMPT_STRING	"PID_%1: %2"
#define GYRO_Z_SP_PROMPT_STRING	"GZSP: %1"
#define MOTOR_LR_PROMPT_STRING	"THR_%1: %2"
#define CLAHE_PROMPT_STRING     "CLAHE: %1"
#define ZOOM_PROMPT_STRING      "ZOOM: %1X"


class VideoWindowOverlay
{
public:
	VideoWindowOverlay();

    void drawRobotData(cv::Mat& frame, const RobotData& robotData);
	void toggleForeground();
	void setEnhancementState(const VideoProcessing::Algorithm algoEnabled);
	void setZoomState(const DigitalZoomStep zoomStep);

private:
	inline void drawImuData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawPidData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawServoData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawMotorData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawEnhancementState(cv::Mat& frame, cv::Point coord);
	inline void drawZoomState(cv::Mat& frame, cv::Point coord);

	double _fontScale;
	int _doublePrecision;

	int _marginW;
	int _marginH;
	int _dX;
	int _dY;
	cv::Scalar _foreground;
	VideoProcessing::Algorithm _algoEnabled;
	DigitalZoomStep _zoomStep;

	
};

#endif
