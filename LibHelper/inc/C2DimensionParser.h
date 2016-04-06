////////////////////////////////////////////////////
// ������ά��İ�����
// Wupeng 
// qq:4398151
// ��ά��ʹ��Unicode littleending���ַ�,��0xff0xfe��ͷ������
// ��һ��Ϊ���������ͺ�˵��, ʹ��(;)�ָ�,
// ��һ���Ǳ��ID
// ÿһ�н�βʹ��\r\n(0d0a)����β
////////////////////////////////////////////////////
#pragma once
#include <Windows.h>
#include <string>
using namespace std;

struct TypeContainer{
	const WCHAR* pTypeName;
	const WCHAR* pMemberStr;
};

class C2DimensionSourceFile
{
public:
	C2DimensionSourceFile();
	bool Generate(wstring &firstMetaRow);

private:
	bool GenerateHeader();
	bool GenerateType();
	bool GenerateRear();
	TypeContainer* GetTypeContaner(LPWSTR lpwTypeName);

private:
	wstring m_wstrMetaRow;
	wstring m_wstrResultContent;
};

class C2DimensionParser
{
public:
	C2DimensionParser(){
		m_RowCount = 0;
		m_ColCount = 0;
	}
	bool LoadTableFromFile(wstring& filePath);
	
private:
	int m_RowCount;
	int m_ColCount;
};