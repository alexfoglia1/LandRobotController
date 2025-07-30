#ifndef VIDEOWINDOWOVERLAY_H
#define VIDEOWINDOWOVERLAY_H

#include <opencv2/opencv.hpp>

#include "RobotData.h"
#include "VideoProcessing.h"
#include "DigitalZoom.h"
#include "Tracker.h"
#include "SharedMemoryStream.h"


#define SERVO_AZI_PROMPT_STRING	"AZI: %1"
#define SERVO_ELE_PROMPT_STRING	"ELE: %1"
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

    void drawRobotData(cv::Mat& frame, const RobotData& robotData, std::vector<SharedMemoryStream::Detection>& detections);
	void toggleForeground();
	void setEnhancementState(const quint32 algoEnabled);
	void setZoomState(const quint8 zoomStep);
	void setTrackerTarget(const struct Tracker::Target& trackerTarget);

private:
	inline void drawImuData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawPidData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawServoData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawMotorData(cv::Mat& frame, const RobotData& robotData, cv::Point coord);
	inline void drawDetections(cv::Mat& frame, std::vector<SharedMemoryStream::Detection>& detections);
	inline void drawEnhancementState(cv::Mat& frame, cv::Point coord);
	inline void drawZoomState(cv::Mat& frame, cv::Point coord);
	inline void drawTrackerData(cv::Mat& frame);

	double _fontScale;
	int _doublePrecision;

	int _marginW;
	int _marginH;
	int _dX;
	int _dY;
	struct Tracker::Target _trackerTarget;
	cv::Scalar _foreground;
	quint32 _algoEnabled;
	DigitalZoomStep _zoomStep;

	
};

#endif
