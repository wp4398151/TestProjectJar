////////////////////////////////////////////////////
// ������ά��İ�����
// Wupeng 
// qq:4398151
// ��ά��ʹ��Unicode���ַ�������
// ��һ��Ϊ���������ͺ�˵��, ʹ��(;)�ָ�,
// ��һ���Ǳ��ID
// ÿһ�н�βʹ��\r\n(0d0a)����β
////////////////////////////////////////////////////
#include "C2DimensionParser.h"
#include "Helper.h"
#include <fstream>

static TypeContainer s_TypeBufferString[] = 
{
	{ L"INT", L"int"},
	{ L"LONG", L"__int64"},
	{ L"STRING", L"wchar_t*"},
	{ L"FLOAT", L"float"},
	{ L"BOOL", L"bool"}
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

bool C2DimensionFile::Generate(wstring& wstrMetaRow, int row, int colounm)
{
	m_RowCount = row;
	m_ColumnCount = colounm;

	m_wstrMetaRow = wstrMetaRow;
	m_wstrMetaRow.replace(m_wstrMetaRow.find(L"\r"), 1, L"\t");
	GenerateHeader();

	GenerateType();
	m_wstrMetaRow.find(L'\t', 0);
	GenerateRear();
	return true;
}

bool C2DimensionFile::GenerateHeader()
{
	wstring wstrTableName = m_wstrMetaRow.substr(0, m_wstrMetaRow.find(L'\t', 0));
	
	m_wstrResultContent =
		L"#pragma once				\r\n"
		L"class ";
	m_wstrResultContent += wstrTableName;
	m_wstrResultContent += L"Table{ \r\n";
	return true;
}

bool C2DimensionSourceFile::GenerateType()
{
	m_wstrResultContent += L"public:\r\n";
	unsigned int curPos = m_wstrMetaRow.find(L'\t', 0)+1;

	while (curPos < m_wstrMetaRow.size())
	{
		wstring wFieldStr = m_wstrMetaRow.substr(curPos, m_wstrMetaRow.find(L'\t', curPos) - curPos);
		int typePos = wFieldStr.find(L':', 0);
		ASSERT_NEQU(typePos, string::npos);
		wstring wstrFieldName = wFieldStr.substr(0,typePos);
		wstring wstrType = wFieldStr.substr(typePos + 1, wFieldStr.size() - (typePos + 1));
		TypeContainer *pTypeContainer = GetTypeContaner((WCHAR*)wstrType.c_str());
		ASSERT_NOTNULL(pTypeContainer);
		m_wstrResultContent += L"\tm_";
		m_wstrResultContent += wstrFieldName;
		m_wstrResultContent += L" ";
		m_wstrResultContent += pTypeContainer->pMemberStr;
		m_wstrResultContent += L";\r\n";
		curPos = m_wstrMetaRow.find(L'\t', curPos) + 1;
	}
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

	// ��ҪLittle Ending ��Unicode
	if (ifs.get() != 0xff || ifs.get() != 0xfe)
	{
		ifs.close();
		return false;
	}

	string strLine;
	getline(ifs, strLine);
	wstring wstrLine;
	CopyStringToWString(wstrLine, strLine);
	// �鿴һ���м���
	m_ColCount = 1;
	for (wstring::iterator itor = wstrLine.begin();
		itor != wstrLine.end(); ++itor)
	{
		if ((*itor) == L'\t')
		{
			++m_ColCount;
		}
	}

	// �鿴һ���м���
	int curPoint = ifs.cur;
	string tmpStr;
	while (!ifs.eof())
	{
		getline(ifs, tmpStr);
		++m_RowCount;
	}
	--m_RowCount;		//��ΪExcel��Ϊunicode���һ��������\0a

	if (bIsIndexTable)
	{
		C2DimensionSourceIndexFile indexFile;
		indexFile.Generate(wstrLine, m_RowCount, m_ColCount);
	}
	else
	{
		// ��һ�и���������Դ�ļ�
		C2DimensionSourceFile sourceFile;
		sourceFile.Generate(wstrLine, m_RowCount, m_ColCount);
	}

	ifs.close();
	return true;
}

// ��Ҫ����
bool C2DimensionSourceIndexFile::GenerateType()
{
	m_wstrResultContent += L"public:\r\n";
	m_wstrResultContent += L"void* [";
	m_wstrResultContent += m_ColumnCount;

	unsigned int curPos = m_wstrMetaRow.find(L'\t', 0)+1;
	
	while (curPos < m_wstrMetaRow.size())
	{
		wstring wFieldStr = m_wstrMetaRow.substr(curPos, m_wstrMetaRow.find(L'\t', curPos) - curPos);
		int typePos = wFieldStr.find(L':', 0);
		ASSERT_NEQU(typePos, string::npos);
		wstring wstrFieldName = wFieldStr.substr(0,typePos);
		wstring wstrType = wFieldStr.substr(typePos + 1, wFieldStr.size() - (typePos + 1));
		TypeContainer *pTypeContainer = GetTypeContaner((WCHAR*)wstrType.c_str());
		ASSERT_NOTNULL(pTypeContainer);
		m_wstrResultContent += L"\tm_";
		m_wstrResultContent += wstrFieldName;
		m_wstrResultContent += L" ";
		m_wstrResultContent += pTypeContainer->pMemberStr;
		m_wstrResultContent += L";\r\n";
		curPos = m_wstrMetaRow.find(L'\t', curPos) + 1;
	}
	return true;
}