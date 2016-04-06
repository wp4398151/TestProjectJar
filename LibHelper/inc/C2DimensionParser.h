////////////////////////////////////////////////////
// 解析二维表的帮助类
// Wupeng 
// qq:4398151
// 二维表使用Unicode littleending宽字符,以0xff0xfe开头来保存
// 第一行为表名和类型和说明, 使用(;)分割,
// 第一列是表的ID
// 每一行结尾使用\r\n(0d0a)来结尾
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