////////////////////////////////////////////////////
// MP4 file parser 
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#include "CMp4FileParser.h"

#include <string>
#include <fstream>

#include "Helper.h"
#include <WinSock2.h>

using namespace std;

std::vector<string> Mp4Box::m_ContanerTypeList = {string("moov"), 
													string("trak"),
													string("mdia"),
													string("minf"),
													string("dinf"),
													string("mp4a"),
													string("stbl")};

#include "gtest/gtest.h"

TEST(CMp4FileParser, TestReadftyp)
{
	CMp4FileParser filePaser;
	wstring wstrFilePath;
	ASSERT_TRUE(GetAppPathW(wstrFilePath));
	wstrFilePath += L"\\test.mp4";
	filePaser.Load(wstrFilePath);
	ASSERT_TRUE(filePaser.IsSucceed());
	Mp4BoxHeader ftypHeader;
	BOOL bIs64 = FALSE;
	filePaser.GetBoxHeader(ftypHeader, bIs64);
	ASSERT_TRUE(filePaser.IsSucceed());
	EXPECT_EQ(ftypHeader.uBoxSize64, 0x20);
	//EXPECT_TRUE(ftypHeader. filePaser.IsSucceed());
}

TEST(CMp4FileParser, TestDumpAllBox)
{
	CMp4FileParser filePaser;
	wstring wstrFilePath;
	ASSERT_TRUE(GetAppPathW(wstrFilePath));
	wstrFilePath += L"\\test.mp4";
	filePaser.Load(wstrFilePath);
	ASSERT_TRUE(filePaser.IsSucceed());
	filePaser.DumpTopLevel();
	//EXPECT_TRUE(ftypHeader. filePaser.IsSucceed());
}

void CMp4FileParser::Load(wstring& mp4filePath)
{
	m_bRes = false;
	DWORD dwFileSize = 0;
	if (!GetFileSizeW(mp4filePath, dwFileSize))
	{
		DOLOGW(L"Get File Size failed!");
		return;
	}
	m_FileContent.resize(dwFileSize);
	ifstream mp4File(mp4filePath, ios_base::in | ios_base::binary);
	if (!mp4File.is_open())
	{
		DOLOGW(L"File Can't Open!");
		return;
	}
	mp4File.seekg(0, ios::beg);
	// the way have a bug, will omit a char
	//std::copy(istream_iterator<unsigned char>(mp4File), istream_iterator<unsigned char>(), m_FileContent.begin());

	char* pData = m_FileContent.data();
	UINT curPos = 0;
	while (!mp4File.eof())
	{
		mp4File.read(pData+curPos, 1024);
		curPos += 1024;
	}

	mp4File.close();
	m_bRes = true;
}

void CMp4FileParser::GetBoxHeader(Mp4BoxHeader& rBoxHeader, BOOL &bIs64, DWORD dwPosition)
{
	m_bRes = false;
	char* pData = m_FileContent.data();

	Mp4BoxHeaderReal boxHeaderReal;
	memcpy_s((char*)&boxHeaderReal, sizeof(Mp4BoxHeaderReal), (char*)(pData + dwPosition), sizeof(boxHeaderReal));
	rBoxHeader.uBoxSize64 = ntohl(boxHeaderReal.uBoxSize);
	rBoxHeader.uBoxType = boxHeaderReal.uBoxType;
	if (rBoxHeader.uBoxSize64 == 1)
	{
		bIs64 = TRUE;
		ULONGLONG ullSize = 0;
		memcpy_s((char*)&ullSize, sizeof(ULONGLONG),(char*)(pData + dwPosition + sizeof(boxHeaderReal)), sizeof(ULONGLONG));
		rBoxHeader.uBoxSize64 = ntohll(ullSize);
	}
	m_bRes = true;
}

void CMp4FileParser::GetBox(Mp4Box& rBox, DWORD dwPosition)
{
	m_bRes = false;
	char* pData = m_FileContent.data();
	BOOL bIs64 = FALSE;
	GetBoxHeader(rBox.boxHeader, bIs64, dwPosition);
	if (!IsSucceed())
	{
		DOLOGW(L"Get Box Header Failed!");
		return;
	}
	if (bIs64)
	{
		rBox.boxHeader.uHeaderLen = sizeof(Mp4BoxHeaderRealEx);
	}
	else
	{
		rBox.boxHeader.uHeaderLen = sizeof(Mp4BoxHeaderReal);
	}

	DWORD dwBoxBodySize = rBox.boxHeader.uBoxSize64 - rBox.boxHeader.uHeaderLen;
	rBox.pBoxBody = new char[dwBoxBodySize];
	memcpy_s(rBox.pBoxBody, dwBoxBodySize, pData + rBox.boxHeader.uHeaderLen + dwPosition, dwBoxBodySize);
	rBox.dwLevel = 1;
	m_bRes = true;
}

bool Mp4Box::Is64Size()
{
	return (boxHeader.uHeaderLen == sizeof(Mp4BoxHeaderRealEx));
}

void Mp4Box::GetSubBoxHeader(Mp4BoxHeader& rBoxHeader, DWORD dwPosition)
{
	Mp4BoxHeaderReal boxHeaderReal;
	memcpy_s(&boxHeaderReal, sizeof(boxHeaderReal), pBoxBody + dwPosition, sizeof(boxHeaderReal));
	rBoxHeader.uBoxSize64 = ntohl(boxHeaderReal.uBoxSize);
	rBoxHeader.uBoxType = boxHeaderReal.uBoxType;
	if (rBoxHeader.uBoxSize64 == 1)
	{
		rBoxHeader.uHeaderLen = sizeof(Mp4BoxHeaderRealEx);
		ULONGLONG ullSize = 0;
		memcpy_s(&ullSize, sizeof(ULONGLONG), pBoxBody + dwPosition + sizeof(boxHeaderReal), sizeof(ULONGLONG));
		rBoxHeader.uBoxSize64 = ntohll(ullSize);
	}
	else
	{
		rBoxHeader.uHeaderLen = sizeof(Mp4BoxHeaderReal);
	}
}

void Mp4Box::GetSubBox(Mp4Box& rBox, DWORD dwPosition)
{
	GetSubBoxHeader(rBox.boxHeader, dwPosition);

	DWORD dwBoxBodySize = rBox.boxHeader.uBoxSize64 - rBox.boxHeader.uHeaderLen;
	rBox.pBoxBody = new char[dwBoxBodySize];
	memcpy_s(rBox.pBoxBody, dwBoxBodySize, pBoxBody + rBox.boxHeader.uHeaderLen + dwPosition, dwBoxBodySize);
	rBox.dwLevel = dwLevel + 1;
}

bool Mp4Box::IsHaveSub()
{
	char buftype[5] = { 0 };
	memcpy_s(buftype, 4, (void*)&(boxHeader.uBoxType), 4);
	std::vector<string>::iterator itor = std::find(m_ContanerTypeList.begin(), m_ContanerTypeList.end(), string(buftype));
	if (itor == m_ContanerTypeList.end())
	{
		return false;
	}
	return true;
}

void Mp4Box::DumpSubBox()
{
	if (!IsHaveSub())
	{
		return;
	}
	DWORD dwTotoal = 0;
	DWORD dwBodyLen = boxHeader.uBoxSize64 - boxHeader.uHeaderLen;
	char buftype[5] = { 0 };
	while (dwTotoal < dwBodyLen)
	{
		Mp4Box box;
		GetSubBox(box, dwTotoal);
		dwTotoal += box.boxHeader.uBoxSize64;
		memcpy_s(buftype, 4, (void*)&(box.boxHeader.uBoxType), 4);
		DOLOG(" " + buftype + " :" + box.dwLevel + ", Current Position:" + dwTotoal);
		box.DumpSubBox(); 
	}
}

void CMp4FileParser::DumpTopLevel()
{
	DOLOG("---------- DumpAllLevel -------------------");
	DWORD dwTotoal = 0;
	char buftype[5] = {0};
	while (dwTotoal < m_FileContent.size())
	{
		Mp4Box box;
		GetBox(box, dwTotoal);
		dwTotoal += box.boxHeader.uBoxSize64;
		memcpy(buftype,(void*)&(box.boxHeader.uBoxType), 4);
		DOLOG(" " + buftype + " :"+ box.dwLevel +", Current Position:" + dwTotoal);
		box.DumpSubBox();
	}

	DOLOG("---------- End Dump -------------------");
	DOLOG("");
}

