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
#include <fstream>

static TypeContainer s_TypeBufferString[] = 
{
	{ L"INT", L"int"},
	{ L"LONG", L"long"},
	{ L"STRING", L"wchar*"},
	{ L"FLOAT", L"float"},
	{ L"BOOL", L"bool"}
};
static int s_TypeBufferCount = sizeof(s_TypeBufferString) / sizeof(s_TypeBufferString[0]);

C2DimensionSourceFile::C2DimensionSourceFile(){ 
}

TypeContainer* C2DimensionSourceFile::GetTypeContaner(LPWSTR lpwTypeName)
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

bool C2DimensionSourceFile::Generate(wstring& wstrMetaRow)
{
	m_wstrMetaRow = wstrMetaRow;
	//m_wstrMetaRow.replace(L'\a', L';');
	m_wstrMetaRow.replace(m_wstrMetaRow.find(L"\r"), 1, L";");
	return true;
}

bool C2DimensionSourceFile::GenerateHeader()
{
	return true;
}

bool C2DimensionSourceFile::GenerateType()
{
	return true;
}

bool C2DimensionSourceFile::GenerateRear()
{
	return true;
}

bool C2DimensionParser::LoadTableFromFile(wstring& filePath)
{
	wifstream ifs;
	ifs.open(filePath, ios_base::in);
	ASSERT_TRUERET(ifs.is_open(), false);
	wstring wstrLine;

	// 需要Little Ending 的Unicode
	if(ifs.get() != 0xff || ifs.get() != 0xfe)
	{
		ifs.close();
		return false;
	}

	getline(ifs, wstrLine);

	// 第一行给用来生成源文件
	C2DimensionSourceFile sourceFile;
	sourceFile.Generate(wstrLine);
	

	// 查看一共有几列
	m_ColCount = 1;
	for (wstring::iterator itor = wstrLine.begin(); 
		itor != wstrLine.end(); ++itor)
	{
		if ((*itor) == L'\t')
		{
			++m_ColCount;
		}
	}
	

	
	// 查看一共有几行
	int curPoint = ifs.cur;
	wstring tmpStrW;
	while (!ifs.eof())
	{
		getline(ifs, tmpStrW);
		++m_RowCount;
	}
	--m_RowCount;		//因为Excel会为unicode最后一行生成是\0a
	ifs.close();
	return true;
}