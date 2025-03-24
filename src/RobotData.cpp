#include "RobotData.h"

RobotData::RobotData() :
	_gyroX(0.0f),
	_gyroY(0.0f),
	_gyroZ(0.0f),
	_accX(0.0f),
	_accY(0.0f),
	_accZ(0.0f),
	_gyroZSp(0),
	_pidP(0.0f),
	_pidI(0.0f),
	_pidD(0.0f),
	_pidU(0.0f),
	_thrL(0),
	_thrR(0),
	_servoAzi(1500),
	_servoEle(1500)
{

}