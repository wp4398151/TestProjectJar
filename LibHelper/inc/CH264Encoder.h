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
	static char* convertRGB2YUV420(char* pBits);	// ��RGB������ת����YUV420��x264�����ָ�ʽ����
	bool Init();
	
};