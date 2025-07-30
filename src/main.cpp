#include <QApplication>

#include "VideoWindow.h"

#include "JoystickBridge.h"
#include "ServoControl.h"
#include "Comm.h"



int main(int argc, char* argv[])
{
    QApplication* app = new QApplication(argc, argv);
    
    VideoWindow* widget = new VideoWindow;
    widget->resize(800, 600);
    widget->show();

    JoystickBridge* js = new JoystickBridge;
    ServoControl* servoControl = new ServoControl(ServoControl::ServoMode::VELOCITY, 2.00, 0.0, 0.5, 1.20, 0.0, 0.5);
#ifdef __HOTSPOT__
    Comm comm("172.20.10.10", 7777);
#else
    Comm* comm = new Comm("192.168.1.6", 7777);
#endif

    QObject::connect(js, &JoystickBridge::updatedThrottle, comm, &Comm::setThrottle);
    QObject::connect(js, &JoystickBridge::updatedGyroZSetPoint, comm, &Comm::setGyroZSetPoint);
    QObject::connect(js, &JoystickBridge::emergencyStop, comm, &Comm::emergencyStop);

    QObject::connect(comm, &Comm::receivedGyroX, widget, &VideoWindow::ReceiveGyroX);
    QObject::connect(comm, &Comm::receivedGyroY, widget, &VideoWindow::ReceiveGyroY);
    QObject::connect(comm, &Comm::receivedGyroZ, widget, &VideoWindow::ReceiveGyroZ);
    QObject::connect(comm, &Comm::receivedAccX, widget, &VideoWindow::ReceiveAccX);
    QObject::connect(comm, &Comm::receivedAccY, widget, &VideoWindow::ReceiveAccY);
    QObject::connect(comm, &Comm::receivedAccZ, widget, &VideoWindow::ReceiveAccZ);
    QObject::connect(comm, &Comm::receivedGyroZSp, widget, &VideoWindow::ReceiveGyroZSp);
    QObject::connect(comm, &Comm::receivedPidP, widget, &VideoWindow::ReceivePidP);
    QObject::connect(comm, &Comm::receivedPidI, widget, &VideoWindow::ReceivePidI);
    QObject::connect(comm, &Comm::receivedPidD, widget, &VideoWindow::ReceivePidD);
    QObject::connect(comm, &Comm::receivedPidU, widget, &VideoWindow::ReceivePidU);
    QObject::connect(comm, &Comm::receivedThrL, widget, &VideoWindow::ReceiveThrL);
    QObject::connect(comm, &Comm::receivedThrR, widget, &VideoWindow::ReceiveThrR);
    QObject::connect(comm, &Comm::receivedServo, widget, &VideoWindow::ReceiveServo);

    QObject::connect(js, &JoystickBridge::emergencyStop, widget, &VideoWindow::EmergencyStop);
    QObject::connect(js, &JoystickBridge::setImageEnhancementAlgorithm, widget, &VideoWindow::SetAlgorithmEnabled);
    QObject::connect(js, &JoystickBridge::setZoomStep, widget, &VideoWindow::SetDigitalZoomStep);

    QObject::connect(js, &JoystickBridge::toggleTracker, widget, &VideoWindow::ToggleTracker);
    QObject::connect(js, &JoystickBridge::stopTracker, widget, &VideoWindow::StopTracker);
    QObject::connect(js, &JoystickBridge::stopTracker, servoControl, &ServoControl::reset);
    QObject::connect(js, &JoystickBridge::stopTracker, comm, [servoControl, comm]
        {
            comm->setServo(quint16(ServoControl::ServoMode::POSITION), 1500, 1500);

            if (servoControl->servoMode() == ServoControl::ServoMode::VELOCITY)
            {
                comm->setServo(quint16(ServoControl::ServoMode::VELOCITY), 1500, 1500);
            }
        });
    QObject::connect(js, &JoystickBridge::enlargeTrackerRoi, widget, &VideoWindow::EnlargeTrackerRoi);
    QObject::connect(js, &JoystickBridge::reduceTrackerRoi, widget, &VideoWindow::ReduceTrackerRoi);

    QObject::connect(servoControl, &ServoControl::updatedServo, comm, &Comm::setServo);
    QObject::connect(js, &JoystickBridge::updatedServo, servoControl, &ServoControl::servoDispatch);
    QObject::connect(widget, &VideoWindow::trackerStarted, servoControl, &ServoControl::reset);
    QObject::connect(widget, &VideoWindow::targetMoved, servoControl, &ServoControl::targetMoved);
    QObject::connect(widget, &VideoWindow::trackerStopped, comm, [servoControl, comm]
    {
        comm->setServo(quint16(ServoControl::ServoMode::POSITION), 1500, 1500);

        if (servoControl->servoMode() == ServoControl::ServoMode::VELOCITY)
        {
            comm->setServo(quint16(ServoControl::ServoMode::VELOCITY), 1500, 1500);
        }
    });

    QObject::connect(js, &JoystickBridge::toggleServoMode, servoControl, [servoControl]
    {
        ServoControl::ServoMode currentMode = servoControl->servoMode();
        servoControl->setMode(currentMode == ServoControl::ServoMode::POSITION ? ServoControl::ServoMode::VELOCITY : ServoControl::ServoMode::POSITION);
    });


    
    QObject::connect(widget, &VideoWindow::trackerCoastingFalure, servoControl, [servoControl] { servoControl->reset(); servoControl->servoDispatch(1500, 1500); });

    comm->start();

    return app->exec();
}
