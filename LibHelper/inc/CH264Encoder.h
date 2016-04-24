////////////////////////////////////////////////////
// 常用帮助函数
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////
#pragma once
#include "Helper.h"
#include <list>
using namespace std;



class CH264Encoder{
public:
	CH264Encoder(){}
	virtual ~CH264Encoder(){}
	CH264Encoder(CH264Encoder& rh264Endoer){}
	
public:
	// 将RGB的内容转换成YUV420，x264以这种格式输入
	static char* ConvertRBGA2YUV420(char* pRgbaBuf, int width, int height);
	static BOOL ConvertRBGA2YUV420Ex(char* pRgbaBuf, int width, int height, char* pYUVBuf);

	static bool TestEncodeScreen();
	static bool TestEncodeScreenUseX264Encoder();
	bool Init();
};

struct DataPacket
{
	LPBYTE lpPacket;
	UINT size;
};
//-------------------------------------------------------------------

class VideoEncoder
{
public:
	virtual bool Encode(LPVOID picInPtr, LPVOID nalOut, int *pNalNum) = 0;
	virtual void RequestBuffers(LPVOID buffers) {}

public:
	virtual ~VideoEncoder() {}

	virtual int  GetBitRate() const = 0;
	virtual bool DynamicBitrateSupported() const = 0;
	virtual bool SetBitRate(DWORD maxBitrate, DWORD bufferSize) = 0;

	virtual void GetHeaders(DataPacket &packet) = 0;
	virtual void GetSEI(DataPacket &packet) {}

	virtual void RequestKeyframe() {}

	virtual string GetInfoString() const = 0;

	virtual bool isQSV() { return false; }

	virtual int GetBufferedFrames() { if (HasBufferedFrames()) return -1; return 0; }
	virtual bool HasBufferedFrames() { return false; }
};

VideoEncoder* CreateX264Encoder(int fps, int width, int height, int quality, int maxBitRate, int bufferSize, bool bUseCFR);
