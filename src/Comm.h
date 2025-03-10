#ifndef COMM_H
#define COMM_H

#define PAYLOAD_SIZE 128
#define EMRG_MSG_ID 0xAA
#define CTRL_MSG_ID 0x7E
#define TLMT_MSG_ID 0x5F

#include <stdint.h>
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer.h>

#pragma pack(push, 1) 

typedef struct
{
    int16_t xAxis;
    int16_t yAxis;
    int16_t throttle;
    uint16_t servo;
    uint16_t checksum;
} CtrlMessage;


typedef union
{
    struct
    {
        uint8_t gx : 1;
        uint8_t gy : 1;
        uint8_t gz : 1;
        uint8_t ax : 1;
        uint8_t ay : 1;
        uint8_t az : 1;
        uint8_t gz_sp : 1;
        uint8_t zero : 1;
    } Bits;
    uint8_t byte;
} TelemetryByte0;


typedef union
{
    struct
    {
        uint8_t pid_p : 1;
        uint8_t pid_i : 1;
        uint8_t pid_d : 1;
        uint8_t pid_u : 1;
        uint8_t thr_l : 1;
        uint8_t thr_r : 1;
        uint8_t servo : 1;
        uint8_t zero : 1;
    } Bits;
    uint8_t byte;
} TelemetryByte1;

typedef struct
{
    TelemetryByte0 byte0;
    TelemetryByte1 byte1;
} TelemetryHeader;

typedef struct
{
    TelemetryHeader header;
    uint8_t payload[PAYLOAD_SIZE];
} TelemetryMessage;


class Comm : public QObject
{
    Q_OBJECT

public:
    Comm(QString destIp, quint16 destPort);

public slots:
    void setXY(qint16 x, qint16 y);
    void setThrottle(qint16 throttle);
    void setServo(quint16 servo);
    void emergencyStop();
    void onSocketReadyRead();
    void onTelemetryTimeout();

    void transmitControl();

signals:
    void receivedGyroX(float gyroX);
    void receivedGyroY(float gyroY);
    void receivedGyroZ(float gyroZ);
    void receivedAccX(float accX);
    void receivedAccY(float accY);
    void receivedAccZ(float accZ);
    void receivedGyroZSp(quint16 gyroZSp);
    void receivedPidP(float gyroZSP);
    void receivedPidI(float gyroZSP);
    void receivedPidD(float gyroZSP);
    void receivedPidU(float gyroZSP);
    void receivedThrL(quint16 thrL);
    void receivedThrR(quint16 thrR);
    void receivedServo(quint16 servo);

private:
    CtrlMessage _ctrlMessage;
    QUdpSocket _sock;
    QHostAddress _addr;
    quint16 _port;
    QTimer _telemetryTimer;
    quint16 _telemetryMask;

    void telemetryIngest(TelemetryMessage* telemetryMessage);
    template <typename T> T popFromPayload(uint8_t* payload, uint32_t* payloadIndex);
    quint16 checksum();
};

#pragma pack(pop)

#endif
