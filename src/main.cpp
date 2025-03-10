#include <QApplication>

#include "VideoWindow.h"

#include "JoystickBridge.h"
#include "Comm.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    VideoWindow widget;
    widget.resize(800, 600);
    widget.show();

    JoystickBridge js;
    Comm comm("192.168.1.7", 7777);


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


    return app.exec();

}
