#ifndef JOYSTICK_BRIDGE_H
#define JOYSTICK_BRIDGE_H
#include <QObject>
#include <QJoysticks.h>

#include "VideoProcessing.h"

class JoystickBridge : public QObject
{
	Q_OBJECT

public:
	JoystickBridge(QObject* parent = nullptr);
	
signals:
	void updatedServo(quint16 servo);
	void updatedThrottle(qint16 throttle);
	void updateXY(qint16 x, qint16 y);
	void emergencyStop();
	void setImageEnhancementAlgorithm(VideoProcessing::Algorithm algo, bool enabled);
	void setZoomStep(quint8 zoomStep);

private:
	QJoysticks* _js;

	quint16 _servoData;
	qint16 _x;
	qint16 _y;
	qint16 _throttle;
	quint8 _zoomStep;
	int _numButtons;
	bool* _buttonsState;

	qreal deadCenterZone(qreal axisValue, qreal deadCenter, qreal dczValue, qreal minAxisValue, qreal maxAxisValue);
	qreal mapValue(qreal value, qreal fromMin, qreal fromMax, qreal toMin, qreal toMax);
	qreal saturate(qreal value, qreal min, qreal max);


private slots:
	void onButtonEvent(const QJoystickButtonEvent& evt);
	void onAxisEvent(const QJoystickAxisEvent& evt);
};

#endif