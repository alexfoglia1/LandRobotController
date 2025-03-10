#include "Comm.h"

#include <windows.h>
#include <iostream>

Comm::Comm(QString destIp, quint16 destPort)
{
	_addr = QHostAddress(destIp);
	_port = destPort;

	memset(&_ctrlMessage, 0x00, sizeof(CtrlMessage));
	connect(&_sock, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));

    _telemetryMask = 0x7F;
    _telemetryTimer.setSingleShot(false);
    _telemetryTimer.setTimerType(Qt::PreciseTimer);
    _telemetryTimer.setInterval(20);
    
    connect(&_telemetryTimer, SIGNAL(timeout()), this, SLOT(onTelemetryTimeout()));
    _telemetryTimer.start();
}


void Comm::onTelemetryTimeout()
{
    TelemetryHeader hdr;

    hdr.byte0.byte = LOBYTE(_telemetryMask);
    hdr.byte1.byte = HIBYTE(_telemetryMask);
    _telemetryMask ^= 0x7F7F;

    QByteArray qba(1 + static_cast<int>(sizeof(TelemetryHeader)), 0);
    qba[0] = (char)(TLMT_MSG_ID);
    memcpy(qba.data() + 1, &hdr, sizeof(TelemetryHeader));
    
    _sock.writeDatagram(qba, _addr, _port);
}



void Comm::setXY(qint16 x, qint16 y)
{
	_ctrlMessage.xAxis = x;
	_ctrlMessage.yAxis = y;

    transmitControl();
}

void Comm::setThrottle(qint16 throttle)
{
	_ctrlMessage.throttle = throttle;

    transmitControl();
}


void Comm::setServo(quint16 servo)
{
	_ctrlMessage.servo = servo;

    //printf("_ctrlMessage.servo(%d)\n", _ctrlMessage.servo);

    transmitControl();
}

void Comm::emergencyStop()
{
    char emrg[1] = { EMRG_MSG_ID };
    _sock.writeDatagram(emrg, 1, _addr, _port);
}


void Comm::transmitControl()
{
	_ctrlMessage.checksum = checksum();

    //std::cout << "xAxis: " << _ctrlMessage.xAxis << std::endl;

	QByteArray qba(1 + static_cast<int>(sizeof(CtrlMessage)), 0x00);
    qba[0] = (char)(CTRL_MSG_ID);
	memcpy(qba.data() + 1, &_ctrlMessage, sizeof(CtrlMessage));

	_sock.writeDatagram(qba, _addr, _port);
}

quint16 Comm::checksum()
{
	quint8* pCtrlMessage = reinterpret_cast<quint8*>(&_ctrlMessage);

	quint8 cks = 0;
	for (size_t i = 0; i < sizeof(CtrlMessage) - sizeof(quint16); i += 1)
	{
		cks ^= pCtrlMessage[i];
	}

	return cks;
}


void Comm::onSocketReadyRead()
{
	TelemetryMessage message;
	while (_sock.hasPendingDatagrams())
	{
		_sock.readDatagram(reinterpret_cast<char*>(&message), sizeof(TelemetryMessage));

		telemetryIngest(&message);
	}
}



void Comm::telemetryIngest(TelemetryMessage* telemetryMessage)
{
    quint32 payloadIndex = 0;
    if (telemetryMessage->header.byte0.Bits.gx)
    {
        //std::cout << "gyroX: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedGyroX(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte0.Bits.gy)
    {
        //std::cout << "gyroY: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedGyroY(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte0.Bits.gz)
    {
        //std::cout << "gyroZ: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedGyroZ(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte0.Bits.ax)
    {
        //std::cout << "accX: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedAccX(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte0.Bits.ay)
    {
        //std::cout << "accY: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedAccY(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte0.Bits.az)
    {
        //std::cout << "accZ: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedAccZ(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte0.Bits.gz_sp)
    {
        //std::cout << "gyroZ setpoint: " << popFromPayload<int16_t>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedGyroZSp(popFromPayload<qint16>(telemetryMessage->payload, &payloadIndex));
    }

    if (telemetryMessage->header.byte1.Bits.pid_p)
    {
        //std::cout << "PID P: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedPidP(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte1.Bits.pid_i)
    {
        //std::cout << "PID I: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedPidI(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte1.Bits.pid_d)
    {
        //std::cout << "PID D: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedPidD(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte1.Bits.pid_u)
    {
        //std::cout << "PID U: " << popFromPayload<float>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedPidU(popFromPayload<float>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte1.Bits.thr_l)
    {
        //std::cout << "THR L: " << popFromPayload<uint16_t>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedThrL(popFromPayload<quint16>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte1.Bits.thr_r)
    {
        //std::cout << "THR R: " << popFromPayload<uint16_t>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedThrR(popFromPayload<quint16>(telemetryMessage->payload, &payloadIndex));
    }
    if (telemetryMessage->header.byte1.Bits.servo)
    {
        //std::cout << "Servo: " << popFromPayload<uint16_t>(telemetryMessage->payload, &payloadIndex) << std::endl;
        emit receivedServo(popFromPayload<quint16>(telemetryMessage->payload, &payloadIndex));
    }
}

template <typename T> T Comm::popFromPayload(uint8_t* payload, uint32_t* payloadIndex)
{
    T val = *reinterpret_cast<T*>(&payload[*payloadIndex]);
    *payloadIndex += sizeof(T);

    return val;
}