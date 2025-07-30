#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

#include <QObject.h>

class ServoControl : public QObject
{
	Q_OBJECT

public:
	enum class ServoMode
	{
		POSITION = 0x00,
		VELOCITY
	};

	ServoControl(ServoMode servoMode, double kpAzi, double kiAzi, double kdAzi, double kpEle, double kiEle, double kdEle, QObject* parent=nullptr);

	void setMode(ServoMode servoMode);
	ServoMode servoMode();

	void setKpAzi(double kp);
	void setKiAzi(double ki);
	void setKdAzi(double kd);
	void setKpEle(double kp);
	void setKiEle(double ki);
	void setKdEle(double kd);

signals:
	void updatedServo(quint16 servoMode, quint16 servoAzi, quint16 servoEle);

public slots:
	void reset();
	void targetMoved(int scartX, int scartY);
	void servoDispatch(quint16 servoAzi, quint16 servoEle);

private:
	double _kpAzi;
	double _kiAzi;
	double _kdAzi;
	double _errorAzi;
	double _derivativeAzi;
	double _integralAzi;
	double _offsetAzi;
	double _kpEle;
	double _kiEle;
	double _kdEle;
	double _errorEle;
	double _derivativeEle;
	double _integralEle;
	double _offsetEle;
	ServoMode _servoMode;

	qint64 _lastMillis;

	double saturate(double in, double outMin, double outMax);
	double toRange(double in, double inMin, double inMax, double outMin, double outMax);
	double pidController(double y, double ysp, double& err, double& integral, double& derivative, double offset, double kp, double ki, double kd, double dt);

	double joyToServo(quint16 joyIn);

};

#endif
