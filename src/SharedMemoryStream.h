#ifndef SHMEM_STREAM_H
#define SHMEM_STREAM_H

#include <Windows.h>
#include <stdint.h>
#include <string>

class SharedMemoryStream 
{
public:

	static const wchar_t* SHM_FRAME_NAME;
	static const wchar_t* SHM_DETS_NAME;

	static const int MAX_DET;
	static const int LABEL_SIZE;
	static const int ENTRY_SIZE;
	static const int DET_SHM_SIZE;
	struct Detection
	{
		std::string label;
		float confidence;
		float x1;
		float y1;
		float x2;
		float y2;
	};

	SharedMemoryStream(int width, int height, int channels=3);

	void sendFrame(uint8_t* frameData);
	int getDetections();
	Detection& detection(int idx);

private:
	HANDLE _hMapFileFrames;
	HANDLE _hMapFileDets;
	Detection* _detections;
	int _frameSize;
};

#endif //SHMEM_STREAM_H