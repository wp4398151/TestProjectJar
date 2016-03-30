////////////////////////////////////////////////////
// 常用帮助函数
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
	static char* convertRGB2YUV420(char* pBits);	// 将RGB的内容转换成YUV420，x264以这种格式输入
	bool Init();
	
};