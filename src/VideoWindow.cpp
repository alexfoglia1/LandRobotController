#include "VideoWindow.h"

#include <QOpenGLTexture>
#include <QImage>
#include <QDebug>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>



VideoWindow::VideoWindow(QWidget* parent)
	: QOpenGLWidget(parent)
{
	//gst-launch-1.0 -v v4l2src device=/dev/video0 ! videoconvert    ! video/x-raw,format=I420,width=640,hei
	// ght=480,framerate=30/1 ! videoflip method=rotate-180 ! x264enc tune=zerolatency bitrate=800 speed-preset=superfast ! rtph264pay ! udpsink host=192.168.1.4 port=5000

	std::string pipeline = "udpsrc port=5000 ! application/x-rtp, encoding-name=H264 ! "
		"rtph264depay ! avdec_h264 ! videoconvert ! appsink";

	// Apri il video stream
	_cap = cv::VideoCapture(pipeline, cv::CAP_GSTREAMER);
	if (!_cap.isOpened())
	{
		std::cerr << "Errore nell'apertura dello stream!" << std::endl;
		return;
	}
	else
	{
		connect(&_timer, &QTimer::timeout, this, &VideoWindow::updateFrame);
		_timer.start(10);
	}

	_lastWidth = 640;
	_lastHeight = 480;

	_processing = new VideoProcessing(VideoProcessing::Format::BGR, VideoProcessing::Format::RGB);
	_tracker = new Tracker();

	connect(_tracker, &Tracker::acquireDone, this, &VideoWindow::onTrackerAcquireDone);
	connect(_tracker, &Tracker::targetMoved, this, &VideoWindow::onTrackerTargetMoved);

	_tracker->start();
}


VideoWindow::~VideoWindow()
{
	makeCurrent();
	glDeleteTextures(1, &_cameraTextureID);
	doneCurrent();
}


void VideoWindow::initializeGL()
{
	initializeOpenGLFunctions();

	// Genera una texture OpenGL
	glGenTextures(1, &_cameraTextureID);
	glBindTexture(GL_TEXTURE_2D, _cameraTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void VideoWindow::updateFrame()
{
	_videoFrame = cv::Mat();

	if (_cap.isOpened())
	{
		_cap >> _videoFrame;
	}

	if (_videoFrame.empty())
	{
		_videoFrame = cv::Mat(_lastHeight, _lastWidth, CV_8UC3);
	}
	else
	{
		_tracker->updateFrame(_videoFrame);
		_processing->process(_videoFrame, _videoFrame);
	}

	_overlay.drawRobotData(_videoFrame, _robotData);
	update();
}


void VideoWindow::updateTexture(GLuint textureId, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, pixels);
}


void VideoWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!_videoFrame.empty())
	{
		updateTexture(_cameraTextureID, GL_RGB, _videoFrame.cols, _videoFrame.rows, GL_RGB, GL_UNSIGNED_BYTE, _videoFrame.data);
		drawTexture(_cameraTextureID);
	}
}


void VideoWindow::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}


void VideoWindow::drawTexture(GLuint textureId)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); glVertex2f(-1, -1);
	glTexCoord2f(1.0, 1.0); glVertex2f(1, -1);
	glTexCoord2f(1.0, 0.0); glVertex2f(1, 1);
	glTexCoord2f(0.0, 0.0); glVertex2f(-1, 1);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


void VideoWindow::ReceiveGyroX(float gyroX)
{
	_robotData.setGyroX(gyroX);
}


void VideoWindow::ReceiveGyroY(float gyroY)
{
	_robotData.setGyroY(gyroY);
}


void VideoWindow::ReceiveGyroZ(float gyroZ)
{
	_robotData.setGyroZ(gyroZ);
}

void VideoWindow::ReceiveAccX(float accX)
{
	_robotData.setAccX(accX);
}


void VideoWindow::ReceiveAccY(float accY)
{
	_robotData.setAccY(accY);
}

void VideoWindow::ReceiveAccZ(float accZ)
{
	_robotData.setAccZ(accZ);
}


void VideoWindow::ReceiveGyroZSp(quint16 gyroZSp)
{
	_robotData.setGyroZSp(gyroZSp);
}


void VideoWindow::ReceivePidP(float pidP)
{
	_robotData.setPidP(pidP);
}


void VideoWindow::ReceivePidI(float pidI)
{
	_robotData.setPidI(pidI);
}


void VideoWindow::ReceivePidD(float pidD)
{
	_robotData.setPidD(pidD);
}


void VideoWindow::ReceivePidU(float pidU)
{
	_robotData.setPidU(pidU);
}


void VideoWindow::ReceiveThrL(quint16 thrL)
{
	_robotData.setThrL(thrL);
}


void VideoWindow::ReceiveThrR(quint16 thrR)
{
	_robotData.setThrR(thrR);
}


void VideoWindow::ReceiveServo(quint16 servo)
{
	_robotData.setServo(servo);
}


void VideoWindow::EmergencyStop()
{
	_overlay.toggleForeground();
	update();
}


void VideoWindow::SetAlgorithmEnabled(VideoProcessing::Algorithm algo, bool enabled)
{
	_processing->setAlgorithmEnabled(algo, enabled);
	_overlay.setEnhancementState(_processing->algorithmsEnabled());
}


void VideoWindow::SetDigitalZoomStep(quint8 zoomStep)
{
	_processing->setDigitalZoomStep(zoomStep);

	_overlay.setEnhancementState(_processing->algorithmsEnabled());
	_overlay.setZoomState(zoomStep);
}


void VideoWindow::ToggleTracker()
{
	Tracker::State currentState = _tracker->state();
	Tracker::State nextState = currentState == Tracker::State::IDLE ? Tracker::State::ACQUIRE :
							   currentState == Tracker::State::ACQUIRE ? Tracker::State::TRACK :
							   Tracker::State::ACQUIRE;

	_tracker->setState(nextState);
}


void VideoWindow::StopTracker()
{
	_tracker->setState(Tracker::State::IDLE);
}


void VideoWindow::EnlargeTrackerRoi()
{
	_tracker->enlargeRoi(20);
}

void VideoWindow::ReduceTrackerRoi()
{
	_tracker->reduceRoi(20);
}


void VideoWindow::onTrackerAcquireDone()
{
	struct Tracker::Target _target = _tracker->target();

	_overlay.setTrackerTarget(_target);
	emit trackerStarted();
}


void VideoWindow::onTrackerTargetMoved()
{
	struct Tracker::Target _target = _tracker->target();

	_overlay.setTrackerTarget(_target);

	emit targetMoved(_target.scartX, _target.scartY);
}