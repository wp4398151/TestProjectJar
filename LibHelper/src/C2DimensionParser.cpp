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

	// ��ҪLittle Ending ��Unicode
	if(ifs.get() != 0xff || ifs.get() != 0xfe)
	{
		ifs.close();
		return false;
	}

	getline(ifs, wstrLine);

	// ��һ�и���������Դ�ļ�
	C2DimensionSourceFile sourceFile;
	sourceFile.Generate(wstrLine);
	

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
	wstring tmpStrW;
	while (!ifs.eof())
	{
		getline(ifs, tmpStrW);
		++m_RowCount;
	}
	--m_RowCount;		//��ΪExcel��Ϊunicode���һ��������\0a
	ifs.close();
	return true;
}