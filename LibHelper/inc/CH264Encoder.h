////////////////////////////////////////////////////
// ���ð�������
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////
#pragma once
#include "Helper.h"

class CH264Encoder{
public:
	CH264Encoder(){}
	virtual ~CH264Encoder(){}
	CH264Encoder(CH264Encoder& rh264Endoer){}
	
public:
	// ��RGB������ת����YUV420��x264�����ָ�ʽ����
	static char* ConvertRBGA2YUV420(char* pRgbaBuf, int width, int height);
	static BOOL ConvertRBGA2YUV420Ex(char* pRgbaBuf, int width, int height, char* pYUVBuf);

	static bool TestEncodeScreen();
	bool Init();
};