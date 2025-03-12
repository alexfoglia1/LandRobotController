#include <QApplication>

#include "VideoWindow.h"

#include "JoystickBridge.h"
#include "Comm.h"


void zoomKernel(int x, int y, uchar3* input, uchar3* output, int width, int height, int zoomX, int zoomY, int zoomWidth, int zoomHeight)
{
    if (x < width && y < height)
    {
        // Calcola le coordinate scalate dalla regione di zoom
        int i = zoomY + (y * zoomHeight) / height;
        int j = zoomX + (x * zoomWidth) / width;

        // Protezione per evitare accessi fuori dai limiti
        i = __min(__max(i, 0), height - 1);
        j = __min(__max(j, 0), width - 1);

        printf("(%d,%d)\t",i, j);
    }
}

int main(int argc, char* argv[]) {
#if 0
    for (int y = 0; y < 10; y++)
    {
        printf("[ ");
        for (int x = 0; x < 10; x++)
        {
            zoomKernel(x, y, nullptr, nullptr, 20, 20, 5, 5, 10, 10);
        }
        printf("]\n");
    }
#else
    QApplication app(argc, argv);
    
    VideoWindow widget;
    widget.resize(800, 600);
    widget.show();

    JoystickBridge js;
#ifdef __HOTSPOT__
    Comm comm("172.20.10.10", 7777);
#else
    Comm comm("192.168.1.7", 7777);
#endif

    QObject::connect(&js, &JoystickBridge::updatedServo, &comm, &Comm::setServo);
    QObject::connect(&js, &JoystickBridge::updatedThrottle, &comm, &Comm::setThrottle);
    QObject::connect(&js, &JoystickBridge::updateXY, &comm, &Comm::setXY);
    QObject::connect(&js, &JoystickBridge::emergencyStop, &comm, &Comm::emergencyStop);

    QObject::connect(&comm, &Comm::receivedGyroX, &widget, &VideoWindow::ReceiveGyroX);
    QObject::connect(&comm, &Comm::receivedGyroY, &widget, &VideoWindow::ReceiveGyroY);
    QObject::connect(&comm, &Comm::receivedGyroZ, &widget, &VideoWindow::ReceiveGyroZ);
    QObject::connect(&comm, &Comm::receivedAccX, &widget, &VideoWindow::ReceiveAccX);
    QObject::connect(&comm, &Comm::receivedAccY, &widget, &VideoWindow::ReceiveAccY);
    QObject::connect(&comm, &Comm::receivedAccZ, &widget, &VideoWindow::ReceiveAccZ);
    QObject::connect(&comm, &Comm::receivedGyroZSp, &widget, &VideoWindow::ReceiveGyroZSp);
    QObject::connect(&comm, &Comm::receivedPidP, &widget, &VideoWindow::ReceivePidP);
    QObject::connect(&comm, &Comm::receivedPidI, &widget, &VideoWindow::ReceivePidI);
    QObject::connect(&comm, &Comm::receivedPidD, &widget, &VideoWindow::ReceivePidD);
    QObject::connect(&comm, &Comm::receivedPidU, &widget, &VideoWindow::ReceivePidU);
    QObject::connect(&comm, &Comm::receivedThrL, &widget, &VideoWindow::ReceiveThrL);
    QObject::connect(&comm, &Comm::receivedThrR, &widget, &VideoWindow::ReceiveThrR);
    QObject::connect(&comm, &Comm::receivedServo, &widget, &VideoWindow::ReceiveServo);

    QObject::connect(&js, &JoystickBridge::emergencyStop, &widget, &VideoWindow::EmergencyStop);
    QObject::connect(&js, &JoystickBridge::setImageEnhancementAlgorithm, &widget, &VideoWindow::SetAlgorithmEnabled);
    QObject::connect(&js, &JoystickBridge::setZoomStep, &widget, &VideoWindow::SetDigitalZoomStep);

    return app.exec();
#endif
}
