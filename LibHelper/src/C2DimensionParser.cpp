////////////////////////////////////////////////////
// 解析二维表的帮助类
// Wupeng 
// qq:4398151
// 二维表使用Unicode宽字符来保存
// 第一行为表名和类型和说明, 使用(;)分割,
// 第一列是表的ID
// 每一行结尾使用\r\n(0d0a)来结尾
////////////////////////////////////////////////////
#include "C2DimensionParser.h"
#include "Helper.h"
#include <list>
#include <fstream>

void GenINTFunc(wstring* orignStr, wstring& resStr);
void GenLONGFunc(wstring* orignStr, wstring& resStr);
void GenSTRINGFunc(wstring* orignStr, wstring& resStr);
void GenBOOLFunc(wstring* orignStr, wstring& resStr);
void GenFLOATunc(wstring* orignStr, wstring& resStr);

static TypeContainer s_TypeBufferString[] = 
{
	{ L"INT", L"int", GenINTFunc},
	{ L"LONG", L"__int64", GenLONGFunc},
	{ L"STRING", L"wchar_t*", GenSTRINGFunc},
	{ L"FLOAT", L"float", GenFLOATunc},
	{ L"BOOL", L"int", GenBOOLFunc}
};
static int s_TypeBufferCount = sizeof(s_TypeBufferString) / sizeof(s_TypeBufferString[0]);

TypeContainer* C2DimensionFile::GetTypeContaner(const WCHAR* lpwTypeName)
{
	ASSERT_NOTNULLRET(lpwTypeName, NULL);
	for (int i = 0; i < s_TypeBufferCount; ++i)
	{
		if (0 == lstrcmpiW(lpwTypeName, s_TypeBufferString[i].pTypeName))
		{
			return &(s_TypeBufferString[i]);
		}
	}
	return NULL;
}

bool C2DimensionFile::Generate(wstring& wstrMetaRow, int row, int colounm, list<wstring>& contentStrList)
{
	m_RowCount = row;
	m_ColumnCount = colounm;

	m_wstrMetaRow = wstrMetaRow;
	m_wstrMetaRow.replace(m_wstrMetaRow.find(L"\r"), 1, L"\t");
	GenerateHeader();

	GenerateType();

	GenerateFill(contentStrList);
	GenerateRear();

	OutputDebugStringW(m_wstrResultContent.c_str());
	return true;
}

bool C2DimensionFile::GenerateHeader()
{
	m_wstrTableName = m_wstrMetaRow.substr(0, m_wstrMetaRow.find(L"\t", 0));
	
	m_wstrResultContent =
		L"#pragma once				\r\n"
		L"class ";
	m_wstrResultContent += m_wstrTableName;
	m_wstrResultContent += L"Table{ \r\n";
	return true;
}

bool C2DimensionSourceFile::GenerateFill(list<wstring>& contentList)
{
	m_wstrResultContent += L"const int ";
	m_wstrResultContent += m_wstrTableName;
	m_wstrResultContent += L"Table::m_Size = ";
	m_wstrResultContent += ItoWStatic(m_RowCount);
	m_wstrResultContent += L";\r\n";
	
	m_wstrResultContent += L"const ";
	m_wstrResultContent += m_wstrTableName;
	m_wstrResultContent += L"::Row ";
	m_wstrResultContent += m_wstrTableName;
	m_wstrResultContent += L"Table::m_Row[";
	m_wstrResultContent += ItoWStatic(m_RowCount);
	m_wstrResultContent += L"]={\r\n";

	for (list<wstring>::iterator itor = contentList.begin(); 
			itor != contentList.end(); ++itor)
	{
		m_wstrResultContent += L"{";
		unsigned int curPos = (*itor).find(L"\t", 0) + 1;

		int colIndex = 0;
		while (curPos < (*itor).size())
		{
			wstring wFieldValueStr = (*itor).substr(curPos, (*itor).find(L"\t", curPos) - curPos);
			wstring wResValueStr;
			m_ConvertFunc[colIndex++](&wFieldValueStr, wResValueStr);
			m_wstrResultContent += wResValueStr;
			m_wstrResultContent += L",";
			curPos = (*itor).find(L"\t", curPos) + 1;
		}
		m_wstrResultContent += L"},\r\n";
	}
	return true;
}

bool C2DimensionSourceFile::GenerateType()
{
	m_ConvertFunc = (ValueFunc*)malloc(m_ColumnCount*sizeof(ValueFunc));
	m_wstrResultContent += L"public:\r\n";
	m_wstrResultContent += L"struct Row{\r\n";

	unsigned int curPos = m_wstrMetaRow.find(L"\t", 0)+1;
	int colIndex = 0;
	while (curPos < m_wstrMetaRow.size())
	{
		wstring wFieldStr = m_wstrMetaRow.substr(curPos, m_wstrMetaRow.find(L"\t", curPos) - curPos);
		int typePos = wFieldStr.find(L":", 0);
		ASSERT_NEQU(typePos, string::npos);
		wstring wstrFieldName = wFieldStr.substr(0,typePos);
		wstring wstrType = wFieldStr.substr(typePos + 1, wFieldStr.size() - (typePos + 1));
		TypeContainer *pTypeContainer = GetTypeContaner((WCHAR*)wstrType.c_str());
		ASSERT_NOTNULL(pTypeContainer);

		m_ConvertFunc[colIndex++] = pTypeContainer->pValueFunc;

		m_wstrResultContent += L"\t";
		m_wstrResultContent += pTypeContainer->pMemberStr;
		m_wstrResultContent += L" ";
		m_wstrResultContent += L"m_";
		m_wstrResultContent += wstrFieldName;
		m_wstrResultContent += L";\r\n";
		curPos = m_wstrMetaRow.find(L"\t", curPos) + 1;
	}
	m_wstrResultContent += L"\tRow m_Row[";
	m_wstrResultContent += ItoWStatic(m_RowCount);
	m_wstrResultContent += L"];\r\n";
	m_wstrResultContent += L"\t const static int m_Size;\r\n";
	m_wstrResultContent += L"};\r\n";
	return true;
}

bool C2DimensionFile::GenerateRear()
{
	m_wstrResultContent += L"}; \r\n";
	return true;
}

bool C2DimensionParser::LoadTableFromFile(wstring& filePath, bool bIsIndexTable)
{
	ifstream ifs;
	ifs.open(filePath, ios_base::in);
	ASSERT_TRUERET(ifs.is_open(), false);

	// 需要Little Ending 的Unicode
	if (ifs.get() != 0xff || ifs.get() != 0xfe)
	{
		ifs.close();
		return false;
	}

	string strLine;
	getline(ifs, strLine);
	ifs.get();	// 多出来一个\0
	wstring wstrLine;
	CopyStringToWString(wstrLine, strLine);
	// 查看一共有几列
	m_ColCount = 0;	// 不包括ID一列
	for (wstring::iterator itor = wstrLine.begin();
		itor != wstrLine.end(); ++itor)
	{
		if ((*itor) == L'\t')
		{
			++m_ColCount;
		}
	}

	// 查看一共有几行
	list<wstring> contentList;
	int curPoint = ifs.cur;
	string tmpStr;
	while (!ifs.eof())
	{
		wstring tmpWstr;
		getline(ifs, tmpStr);
		ifs.get();	// 多出来一个\0
		if (tmpStr.size() == 0)//因为Excel会为unicode最后一行生成是\0a
		{
			break;
		}
		CopyStringToWString(tmpWstr, tmpStr);
		tmpWstr.replace(tmpWstr.find(L"\r"), 1, L"\t");
		contentList.push_back(tmpWstr);
		++m_RowCount;
	}

	if (bIsIndexTable)
	{
		C2DimensionSourceIndexFile indexFile;
		indexFile.Generate(wstrLine, m_RowCount, m_ColCount, contentList);
	}
	else
	{
		// 第一行给用来生成源文件
		C2DimensionSourceFile sourceFile;
		sourceFile.Generate(wstrLine, m_RowCount, m_ColCount, contentList);
	}

	ifs.close();
	return true;
}

bool C2DimensionSourceIndexFile::GenerateFill(list<wstring>& contentList)
{
	m_wstrResultContent += L"const int ";
	m_wstrResultContent += m_wstrTableName;
	m_wstrResultContent += L"Table::m_Size = ";
	m_wstrResultContent += ItoWStatic(m_RowCount);
	m_wstrResultContent += L";\r\n";


	return true;
}

bool C2DimensionSourceIndexFile::GenerateType()
{
	m_wstrResultContent += L"public:\r\n";
	m_wstrResultContent += L"void* m_pColounm[";
	m_wstrResultContent += ItoWStatic(m_ColumnCount);
	m_wstrResultContent += L"];\r\n";

	unsigned int curPos = m_wstrMetaRow.find(L"\t", 0) + 1;

	while (curPos < m_wstrMetaRow.size())
	{
		wstring wFieldStr = m_wstrMetaRow.substr(curPos, m_wstrMetaRow.find(L"\t", curPos) - curPos);
		int typePos = wFieldStr.find(L":", 0);
		ASSERT_NEQU(typePos, string::npos);
		wstring wstrFieldName = wFieldStr.substr(0, typePos);
		wstring wstrType = wFieldStr.substr(typePos + 1, wFieldStr.size() - (typePos + 1));
		TypeContainer *pTypeContainer = GetTypeContaner((WCHAR*)wstrType.c_str());
		ASSERT_NOTNULL(pTypeContainer);

		m_wstrResultContent += L"\t";
		m_wstrResultContent += pTypeContainer->pMemberStr;
		m_wstrResultContent += L" m_";
		m_wstrResultContent += wstrFieldName;
		m_wstrResultContent += L"[";
		m_wstrResultContent += ItoWStatic(m_RowCount);
		m_wstrResultContent += L"];\r\n";
		curPos = m_wstrMetaRow.find(L"\t", curPos) + 1;
	}

	m_wstrResultContent += L"\t const static int m_Size;\r\n";
	m_wstrResultContent += L"\t template<typename T>\r\n";
	m_wstrResultContent += L"\t static bool GetCell(int x, int y, T& res){\r\n";
	m_wstrResultContent += L"if (ISINRANGE(x, 0, ";
	m_wstrResultContent += ItoWStatic(m_RowCount-1);
	m_wstrResultContent += L") && ISINRANGE(y, 0, ";
	m_wstrResultContent += ItoWStatic(m_ColumnCount - 1);
	m_wstrResultContent += L")){\r\n";
	m_wstrResultContent += L"res = ((T*)s_pColoun[y])[x];\r\n";
	m_wstrResultContent += L"return true;";
	m_wstrResultContent += L"}else{\r\n";
	m_wstrResultContent += L"return false; }\r\n";

	m_wstrResultContent += L"};\r\n";
	
	return true;
}
// different type different record
void GenINTFunc(wstring* orignStr, wstring& resStr) 
{
	if (orignStr == NULL || orignStr->size()==0){
		resStr = L"0";
	}else{
		resStr = *orignStr;
	}
}

void GenLONGFunc(wstring* orignStr, wstring& resStr)
{
	GenINTFunc(orignStr, resStr);
}

void GenSTRINGFunc(wstring* orignStr, wstring& resStr)
{
	if (orignStr == NULL || orignStr->size()==0){
		resStr += L"NULL";
	}else{
		resStr = L"L\"";
		resStr += *orignStr;
		resStr += L"\"";
	}
}

void GenBOOLFunc(wstring* orignStr, wstring& resStr)
{
	if (orignStr != NULL && orignStr->size() !=0 &&
			(orignStr->c_str()[0] == L'1')){
		resStr = L"1";
	}else{
		resStr = L"0";
	}
}

void GenFLOATunc(wstring* orignStr, wstring& resStr)
{
	if (orignStr == NULL || orignStr->size() == 0){
		resStr += L"0.0";
	}else if (string::npos == orignStr->find(L".")){
		resStr += *orignStr;
		resStr += L".0";
	}else{
		resStr += *orignStr;
	}
	resStr += L"f";
}