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


    return app.exec();

}
