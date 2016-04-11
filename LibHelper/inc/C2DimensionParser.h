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