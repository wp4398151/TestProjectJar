#include "SystemClass.h"
#include "UnitTest.h"
#include "TestTable.h"

//Ӧ�ó������main����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	// ���в���
	UnitTest::Instance().testAll();
	return 0;
	SystemClass* System;
	bool result;

	// ����һ��system����.
	System = new SystemClass;
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

