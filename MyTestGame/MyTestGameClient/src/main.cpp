#include "SystemClass.h"
#include "UnitTest.h"
#include "CH264Encoder.h"
#include "UserTable.h"
#include "UserIndexTable.h"

void TestGenerateDataTable()
{
	wstring appPath;
	wstring outputTableFile;
	assert(GetAppPathW(appPath));
	outputTableFile = (appPath + L"\\..\\MyTestGameClient\\inc\\GenerateTables\\RoleTable.h");
	appPath += L"\\Role.txt";
	C2DimensionParser paser;
	paser.LoadTableFromFile(appPath, outputTableFile, false);
}

//Ӧ�ó������main����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{

//	CH264Encoder::TestEncodeScreen();
	//TestGenerateDataTable();
	//return true;

	//int res = 0;
	//UserIndexTable::GetCell(1, 1, res);
	// ���в���
	//UnitTest::Instance().testAll();
	//return 0;	SystemClass* System;
	bool result;

	// ����һ��system����.
	SystemClass* System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// ��ʼ���Լ�����system����.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// �ر��Լ��ͷ�system����.
	System->Shutdown();
	delete System;
	System = 0;
	return 0;
}