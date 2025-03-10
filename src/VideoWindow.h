#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <opencv2/videoio/videoio.hpp>

#include "VideoWindowOverlay.h"
#include "RobotData.h"
#include "VideoProcessing.h"


class VideoWindow : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit VideoWindow(QWidget* parent = nullptr);
    ~VideoWindow();

public slots:
    void ReceiveGyroX(float gyroX);
    void ReceiveGyroY(float gyroY);
    void ReceiveGyroZ(float gyroZ);
    void ReceiveAccX(float accX);
    void ReceiveAccY(float accY);
    void ReceiveAccZ(float accZ);
    void ReceiveGyroZSp(quint16 gyroZSp);
    void ReceivePidP(float pidP);
    void ReceivePidI(float pidI);
    void ReceivePidD(float pidD);
    void ReceivePidU(float pidU);
    void ReceiveThrL(quint16 thrL);
    void ReceiveThrR(quint16 thrR);
    void ReceiveServo(quint16 servo);
    void EmergencyStop();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private slots:
    void updateFrame();  // Slot per aggiornare lo stream

private:
    QTimer _timer;         // Timer per aggiornare i frame
    cv::VideoCapture _cap; // Stream UDP
    GLuint _cameraTextureID;     // Texture OpenGL video
    cv::Mat _videoFrame;        // Frame video corrente
    RobotData _robotData;
    VideoWindowOverlay _overlay;
    VideoProcessing* _processing;

    void updateTexture(GLuint textureId, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
    void drawTexture(GLuint textureId);
};

#endif
