#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <opencv2/videoio/videoio.hpp>

class VideoWindow : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit VideoWindow(QWidget* parent = nullptr);
    ~VideoWindow();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private slots:
    void updateFrame();  // Slot per aggiornare lo stream

private:
    QTimer _timer;         // Timer per aggiornare i frame
    cv::VideoCapture _cap; // Stream RTSP
    GLuint _cameraTextureID;     // Texture OpenGL video
    cv::Mat _videoFrame;        // Frame video corrente

    void updateTexture(GLuint textureId, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
    void drawTexture(GLuint textureId);
};

#endif
