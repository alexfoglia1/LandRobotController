#ifndef JOYSTICK_BRIDGE_H
#define JOYSTICK_BRIDGE_H
#include <QObject>
#include <QJoysticks.h>

class JoystickBridge : public QObject
{
	Q_OBJECT
public:
	JoystickBridge(QObject* parent = nullptr);
	
signals:
	void updatedServo(quint16 servo);
	void updatedThrottle(qint16 throttle);
	void updateXY(qint16 x, qint16 y);
private:
	QJoysticks* _js;

	quint16 _servoData;
	qint16 _x;
	qint16 _y;
	qint16 _throttle;

	qreal deadCenterZone(qreal axisValue, qreal deadCenter, qreal dczValue, qreal minAxisValue, qreal maxAxisValue);
	qreal mapValue(qreal value, qreal fromMin, qreal fromMax, qreal toMin, qreal toMax);
	qreal saturate(qreal value, qreal min, qreal max);


private slots:
	void onButtonEvent(const QJoystickButtonEvent& evt);
	void onAxisEvent(const QJoystickAxisEvent& evt);
};

#endif