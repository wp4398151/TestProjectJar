#include "SystemClass.h"
#include "UnitTest.h"
#include "CH264Encoder.h"
#include "UserTable.h"
#include "UserIndexTable.h"

//应用程序入口main函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	//CH264Encoder::TestEncodeScreen();
	//int res = 0;
	//UserIndexTable::GetCell(1, 1, res);
	// 进行测试
	//UnitTest::Instance().testAll();
	//return 0;
	SystemClass* System;
	bool result;

	// 创建一个system对象.
	System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// 初始化以及运行system对象.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// 关闭以及释放system对象.
	System->Shutdown();
	delete System;
	System = 0;
	return 0;
}