/**
 * @file
 * @brief 解析二维表的帮助类
 *
 * @author Wupeng <mailto:4398151@qq.com>
 *
 * 二维表使用Unicode littleending宽字符,以0xff0xfe开头来保存
 * 第一行为表名和类型和说明, 使用(;)分割,
 * 第一列是表的ID
 * 每一行结尾使用\r\n(0d0a)来结尾
 */

#pragma once
#include "Helper.h"
#include <Windows.h>
#include <string>
#include <list>
using namespace std;

typedef void (*ValueFunc) (wstring* orignStr, wstring& resStr);

struct TypeContainer{
	const WCHAR* pTypeName;
	const WCHAR* pMemberStr;
	const ValueFunc pValueFunc;
};

class C2DimensionFile{
public:
	C2DimensionFile();
	~C2DimensionFile();
	bool Generate(wstring &firstMetaRow, int row, int colounm, list<wstring>& contentStrList);
	wstring *GetResult(){ return &m_wstrResultContent; }
protected:
	bool GenerateHeader();
	virtual bool GenerateType(wstring* wstrColoumnName) = 0;
	virtual bool GenerateFill(wstring* wstrColoumnName, list<wstring>& contentList) = 0;

	TypeContainer* GetTypeContaner(const WCHAR* lpwTypeName);
protected:
	wstring m_wstrMetaRow;
	wstring m_wstrResultContent;
	wstring m_wstrTableName;
	int m_RowCount;
	int m_ColumnCount;
	ValueFunc *m_ConvertFunc;
};

class C2DimensionSourceFile : public C2DimensionFile
{
public:
	bool GenerateType(wstring* wstrColoumnName);
	bool GenerateFill(wstring* wstrColoumnName, list<wstring>& contentList);
};

class C2DimensionSourceIndexFile : public C2DimensionFile
{
public:
	bool GenerateType(wstring* wstrColoumnName);
	bool GenerateFill(wstring* wstrColoumnName, list<wstring>& contentList);
};

class C2DimensionParser
{
public:
	C2DimensionParser(){
		m_RowCount = 0;
		m_ColCount = 0;
	}
	bool LoadTableFromFile(wstring& filePath, wstring& fileOutpath, bool bIsIndexFile);
	
private:
	int m_RowCount;
	int m_ColCount;
};