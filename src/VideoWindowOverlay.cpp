#include "VideoWindowOverlay.h"

#include <QString>

VideoWindowOverlay::VideoWindowOverlay() :
	_doublePrecision(2),
	_fontScale(0.6f),
	_marginH(20),
	_marginW(10),
	_dX(10),
	_dY(20),
	_foreground(0, 255, 0),
	_algoEnabled(0),
	_zoomStep(DigitalZoomStep::ZOOM_2X)
{

}

void VideoWindowOverlay::drawRobotData(cv::Mat& frame, const RobotData& robotData, std::vector<SharedMemoryStream::Detection>& detections)
{
	int width = frame.cols;
	int height = frame.rows;

	drawImuData(frame, robotData, cv::Point(_marginW, _marginH));
	drawPidData(frame, robotData, cv::Point(width - _marginW - (strlen(PID_PIDU_PROMPT_STRING) + _doublePrecision) * _dX, _marginH));
	drawServoData(frame, robotData, cv::Point(_marginW, height - _marginH - (2 * _dY)));
	drawMotorData(frame, robotData, cv::Point(width - _marginW - (strlen(PID_PIDU_PROMPT_STRING) + _doublePrecision) * _dX, height - _marginH - (2 * _dY)));

	if (_trackerTarget.state != Tracker::State::IDLE)
	{
		drawTrackerData(frame);
	}
	else
	{
		drawDetections(frame, detections);
	}
}


void VideoWindowOverlay::toggleForeground()
{
	if (_foreground == cv::Scalar(0, 255, 0))
	{
		_foreground = cv::Scalar(255, 0, 0);
	}
	else
	{
		_foreground = cv::Scalar(0, 255, 0);
	}
}

void VideoWindowOverlay::setEnhancementState(const quint32 algoEnabled)
{
	_algoEnabled = algoEnabled;
}

void VideoWindowOverlay::setZoomState(const quint8 zoomStep)
{
	_zoomStep = static_cast<DigitalZoomStep>(zoomStep);
}


void VideoWindowOverlay::setTrackerTarget(const struct Tracker::Target& trackerTarget)
{
	_trackerTarget = { trackerTarget.cx, trackerTarget.cy, trackerTarget.scartX, trackerTarget.scartY, trackerTarget.width, trackerTarget.height, trackerTarget.valid, trackerTarget.correlation, trackerTarget.contrastIdx, trackerTarget.state };
}


inline void VideoWindowOverlay::drawImuData(cv::Mat& frame, const RobotData& robotData, cv::Point coord)
{
	QString accX = QString(ACC_XYZ_PROMPT_STRING).arg("X").arg(QString::number(robotData.getAccX(), (char)103, _doublePrecision));
	QString accY = QString(ACC_XYZ_PROMPT_STRING).arg("Y").arg(QString::number(robotData.getAccY(), (char)103, _doublePrecision));
	QString accZ = QString(ACC_XYZ_PROMPT_STRING).arg("Z").arg(QString::number(robotData.getAccZ(), (char)103, _doublePrecision));
	QString gyrX = QString(GYRO_XYZ_PROMPT_STRING).arg("X").arg(QString::number(robotData.getGyroX(), (char)103, _doublePrecision));
	QString gyrY = QString(GYRO_XYZ_PROMPT_STRING).arg("Y").arg(QString::number(robotData.getGyroY(), (char)103, _doublePrecision));
	QString gyrZ = QString(GYRO_XYZ_PROMPT_STRING).arg("Z").arg(QString::number(robotData.getGyroZ(), (char)103, _doublePrecision));

	cv::putText(frame, gyrZ.toStdString(), cv::Point(coord.x, coord.y), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, gyrY.toStdString(), cv::Point(coord.x, coord.y + _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, gyrX.toStdString(), cv::Point(coord.x, coord.y + 2 * _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, accZ.toStdString(), cv::Point(coord.x, coord.y + 3 * _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, accY.toStdString(), cv::Point(coord.x, coord.y + 4 * _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, accX.toStdString(), cv::Point(coord.x, coord.y + 5 * _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
}


inline void VideoWindowOverlay::drawPidData(cv::Mat& frame, const RobotData& robotData, cv::Point coord)
{
	QString gzSP = QString(GYRO_Z_SP_PROMPT_STRING).arg(QString::number(robotData.getGyroZSp()));
	QString pidP = QString(PID_PIDU_PROMPT_STRING).arg("P").arg(QString::number(robotData.getPidP(), (char)103, _doublePrecision));
	QString pidI = QString(PID_PIDU_PROMPT_STRING).arg("I").arg(QString::number(robotData.getPidI(), (char)103, _doublePrecision));
	QString pidD = QString(PID_PIDU_PROMPT_STRING).arg("D").arg(QString::number(robotData.getPidD(), (char)103, _doublePrecision));
	QString pidU = QString(PID_PIDU_PROMPT_STRING).arg("U").arg(QString::number(robotData.getPidU(), (char)103, _doublePrecision));

	cv::putText(frame, gzSP.toStdString(), cv::Point(coord.x, coord.y), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, pidP.toStdString(), cv::Point(coord.x, coord.y + _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, pidI.toStdString(), cv::Point(coord.x, coord.y + 2 * _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, pidD.toStdString(), cv::Point(coord.x, coord.y + 3 * _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, pidU.toStdString(), cv::Point(coord.x, coord.y + 4 * _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
}


inline void VideoWindowOverlay::drawServoData(cv::Mat& frame, const RobotData& robotData, cv::Point coord)
{
	QString servoAzi = QString(SERVO_AZI_PROMPT_STRING).arg(QString::number(robotData.getServoAzi()));
	QString servoEle = QString(SERVO_ELE_PROMPT_STRING).arg(QString::number(robotData.getServoEle()));

	cv::putText(frame, servoAzi.toStdString(), cv::Point(coord.x, coord.y), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, servoEle.toStdString(), cv::Point(coord.x, coord.y + _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
}


inline void VideoWindowOverlay::drawMotorData(cv::Mat& frame, const RobotData& robotData, cv::Point coord)
{
	QString thrL = QString(MOTOR_LR_PROMPT_STRING).arg("L").arg(QString::number(robotData.getThrL()));
	QString thrR = QString(MOTOR_LR_PROMPT_STRING).arg("R").arg(QString::number(robotData.getThrR()));

	cv::putText(frame, thrL.toStdString(), cv::Point(coord.x, coord.y), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, thrR.toStdString(), cv::Point(coord.x, coord.y + _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
}

inline void VideoWindowOverlay::drawDetections(cv::Mat& frame, std::vector<SharedMemoryStream::Detection>& detections)
{
	for (SharedMemoryStream::Detection& d : detections)
	{
		cv::rectangle(frame, cv::Point((int)d.x1, (int)d.y1), cv::Point((int)d.x2, (int)d.y2), cv::Scalar(255, 255, 0), 2);
		std::string label_text = d.label + " " + std::to_string(int(d.confidence * 100)) + "%";
		int baseLine = 0;
		cv::Size labelSize = cv::getTextSize(label_text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
		int top = std::max<int>((int)d.y1, labelSize.height);
		cv::putText(frame, label_text, cv::Point((int)d.x1, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 2);
	}
}


inline void VideoWindowOverlay::drawTrackerData(cv::Mat& frame)
{
	float digitalZoomScale = (_algoEnabled & static_cast<quint32>(VideoProcessing::Algorithm::DIGITAL_ZOOM) ? getDigitalZoomScale(_zoomStep) : 1.0f);

	cv::Rect targetRect = cv::Rect( _trackerTarget.cx - (_trackerTarget.width) * digitalZoomScale / 2,
									_trackerTarget.cy - (_trackerTarget.height) * digitalZoomScale / 2,
									_trackerTarget.width * digitalZoomScale,
									_trackerTarget.height * digitalZoomScale);

	QString correlation = QString("CORR: %1").arg(_trackerTarget.correlation);
	QString contrastIdx = QString("CTRS: %1").arg(_trackerTarget.contrastIdx);

	cv::putText(frame, correlation.toStdString(), cv::Point(targetRect.x, targetRect.y - _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);
	cv::putText(frame, contrastIdx.toStdString(), cv::Point(targetRect.x, targetRect.y + targetRect.height + _dY), cv::FONT_HERSHEY_SIMPLEX, _fontScale, _foreground, 2);

	if (_trackerTarget.state == Tracker::State::ACQUIRE)
	{
		cv::rectangle(frame, targetRect, _trackerTarget.valid ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 0));
	}
	else if (_trackerTarget.state == Tracker::State::TRACK)
	{
		cv::rectangle(frame, targetRect, cv::Scalar(0, 255, 0));
	}
	else if (_trackerTarget.state == Tracker::State::COAST)
	{
		cv::rectangle(frame, targetRect, cv::Scalar(255, 255, 0));
	}
}