#include "SharedMemoryStream.h"

#include <iostream>

const wchar_t* SharedMemoryStream::SHM_FRAME_NAME = L"Local\\shm_yolo_frame";
const wchar_t* SharedMemoryStream::SHM_DETS_NAME = L"Local\\shm_yolo_dets";

const int SharedMemoryStream::MAX_DET = 100;
const int SharedMemoryStream::LABEL_SIZE = 32;
const int SharedMemoryStream::ENTRY_SIZE = SharedMemoryStream::LABEL_SIZE + 5 * sizeof(float);
const int SharedMemoryStream::DET_SHM_SIZE = 1 + sizeof(int) + SharedMemoryStream::MAX_DET * SharedMemoryStream::ENTRY_SIZE;

SharedMemoryStream::SharedMemoryStream(int width, int height, int channels)
{

    _frameSize = width * height * channels;

	_hMapFileFrames = CreateFileMappingW(
                INVALID_HANDLE_VALUE,
                NULL,
                PAGE_READWRITE,
                0,
                _frameSize + 1,
                SHM_FRAME_NAME);

    _hMapFileDets = OpenFileMappingW(FILE_MAP_READ, FALSE, SHM_DETS_NAME);

    _detections = new SharedMemoryStream::Detection[SharedMemoryStream::MAX_DET];
    memset(_detections, 0x00, sizeof(SharedMemoryStream::Detection) * SharedMemoryStream::MAX_DET);
}

void SharedMemoryStream::sendFrame(uint8_t* frameData)
{
    if (_hMapFileFrames == NULL)
    {
        std::cerr << "Shared memory not opened" << std::endl;
        return;
    }

    LPVOID pBuf = MapViewOfFile(_hMapFileFrames, FILE_MAP_ALL_ACCESS, 0, 0, _frameSize + 1);

    if (pBuf == NULL)
    {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        return;
    }

   
    std::uint8_t* buffer = reinterpret_cast<std::uint8_t*>(pBuf);
    buffer[0] = 0;
    std::memcpy(buffer + 1, frameData, _frameSize);
    buffer[0] = 1;
    

    UnmapViewOfFile(pBuf);
}


int SharedMemoryStream::getDetections()
{
    if (_hMapFileDets == NULL)
    {
        _hMapFileDets = OpenFileMappingW(FILE_MAP_READ, FALSE, SHM_DETS_NAME);
        return 0;
    }
    else
    {
        char* data = (char*)MapViewOfFile(_hMapFileDets, FILE_MAP_READ, 0, 0, DET_SHM_SIZE);
        if (!data)
        {
            std::cerr << "Failed to map view of file for detections: " << GetLastError() << std::endl;
            return -1;
        }

        if (data[0] != 1)
        {
            UnmapViewOfFile(data);
            std::cout << "No detections\n";
            return -1;
        }

        int count;
        std::memcpy(&count, data + 1, sizeof(int));
        count = std::min<int>(count, MAX_DET);

        for (int i = 0; i < count; ++i)
        {
            char label_buf[LABEL_SIZE + 1] = { 0 };
            std::memcpy(label_buf, data + 5 + i * ENTRY_SIZE, LABEL_SIZE);
            float conf;
            std::memcpy(&conf, data + 5 + i * ENTRY_SIZE + LABEL_SIZE, sizeof(float));
            float bbox[4] = { 0, 0, 0, 0 };
            std::memcpy(bbox, data + 5 + i * ENTRY_SIZE + LABEL_SIZE + sizeof(float), 4 * sizeof(float));
            _detections[i].label = std::string(label_buf);
            _detections[i].confidence = conf;
            _detections[i].x1 = bbox[0];
            _detections[i].y1 = bbox[1];
            _detections[i].x2 = bbox[2];
            _detections[i].y2 = bbox[3];
        }

        UnmapViewOfFile(data);

        return count;
    }
}


SharedMemoryStream::Detection& SharedMemoryStream::detection(int idx)
{
    idx = std::min<int>(idx, SharedMemoryStream::MAX_DET);
    return _detections[idx];
}