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

class C2DimensionFile{
public:
	bool Generate(wstring &firstMetaRow, int row, int colounm);

protected:
	bool GenerateHeader();
	virtual bool GenerateType() = 0;
	bool GenerateRear();
	TypeContainer* GetTypeContaner(const WCHAR* lpwTypeName);

protected:
	wstring m_wstrMetaRow;
	wstring m_wstrResultContent;
	int m_RowCount;
	int m_ColumnCount;
};

class C2DimensionSourceFile : public C2DimensionFile
{
public:
	bool GenerateType();
};

class C2DimensionSourceIndexFile : public C2DimensionFile
{
public:
	bool GenerateType();
};

class C2DimensionParser
{
public:
	C2DimensionParser(){
		m_RowCount = 0;
		m_ColCount = 0;
	}
	bool LoadTableFromFile(wstring& filePath, bool bIsIndexFile);
	
private:
	int m_RowCount;
	int m_ColCount;
};