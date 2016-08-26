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
std::vector<BoxHandlerBase*> Mp4Box::m_HandlerList 
= { new BoxMDHDHanlder(), 
	new BoxHDLRHanlder(), 
	new BoxTKHDHanlder(), 
	new BoxMVHDHanlder(),
	new BoxSTSCHanlder(),
};


std::vector<string> Mp4Box::m_ContanerTypeList = {string("moov"), 
													string("trak"),
													string("mdia"),
													string("minf"),
													string("dinf"),
													string("mp4a"),
													string("stbl")};

void BoxSTSCHanlder::DumpInfo(Mp4Box& rBox)
{
	//DWORD dwTotalLen = rBox.boxHeader.uBoxSize64 - rBox.boxHeader.uHeaderLen;
	BoxStructHeader headInfo;
	memcpy_s(&headInfo, sizeof(headInfo), rBox.pBoxBody, sizeof(headInfo));
	COUNTTAB(rBox.dwLevel);

	headInfo.dwEntryCount = ntohl(headInfo.dwEntryCount);
	DWORD dwCurPos = sizeof(headInfo);
	DOLOGS("[ ");
	for (int i = 0; i < headInfo.dwEntryCount ; ++i )
	{
		BoxBody bodyInfo;
		memcpy_s(&bodyInfo, sizeof(bodyInfo), rBox.pBoxBody + dwCurPos, sizeof(bodyInfo));
		dwCurPos += sizeof(bodyInfo);
		bodyInfo.dwFirstChunk = ntohl(bodyInfo.dwFirstChunk);
		bodyInfo.dwSamplePerChunk= ntohl(bodyInfo.dwSamplePerChunk);
		bodyInfo.dwSampleDescriptionIndex = ntohl(bodyInfo.dwSampleDescriptionIndex);
		DOLOGS("( first:" + bodyInfo.dwFirstChunk + ", SamplePerChunk:" 
					+ bodyInfo.dwSamplePerChunk + ", Index:" + bodyInfo.dwSampleDescriptionIndex + ")");
	}
	DOLOGS("] ");
}

void BoxMVHDHanlder::DumpInfo(Mp4Box& rBox)
{
	BoxStruct info;
	memcpy_s(&info, sizeof(info), rBox.pBoxBody, sizeof(info));

	info.dwRate = ntohl(info.dwRate);
	COUNTTAB(rBox.dwLevel);
	DOLOGS(" <this rate is " + info.dwRate + ">");
}

void BoxTKHDHanlder::DumpInfo(Mp4Box& rBox)
{
	BoxStruct info;
	memcpy_s(&info, sizeof(info), rBox.pBoxBody, sizeof(info));
	info.dwWidth = ntohl(info.dwWidth);
	info.dwHeight = ntohl(info.dwHeight);
	
	if (info.dwWidth != 0)
	{
		COUNTTAB(rBox.dwLevel);
		DOLOGS(" <this vedio size is " + (info.dwWidth >>16) + "*" + (info.dwHeight>>16) + ">");
	}
}

void BoxMDHDHanlder::DumpInfo(Mp4Box& rBox)
{
	BoxStruct info;
	memcpy_s(&info, sizeof(info), rBox.pBoxBody, sizeof(info));
	info.dwTimeDuration = ntohl(info.dwTimeDuration);
	info.dwTimeScale = ntohl(info.dwTimeScale);
	COUNTTAB(rBox.dwLevel);
	DOLOGS("<this track time length is " + (info.dwTimeDuration/info.dwTimeScale) + " second >");
};

void BoxHDLRHanlder::DumpInfo(Mp4Box& rBox)
{
	BoxStruct info;
	memcpy_s(&info, sizeof(info), rBox.pBoxBody, sizeof(info));
	COUNTTAB(rBox.dwLevel);
	if (0 == memcmp((char*)(&info.dwHandlerType) ,"vide", 4))
	{
		DOLOGS(" <this track is video>");
	}
	else if(0 == memcmp((char*)(&info.dwHandlerType), "soun", 4))
	{
		DOLOGS(" <this track is audio>");
	}
	else if (0 ==memcmp((char*)(&info.dwHandlerType), "hint", 4))
	{
		DOLOGS(" <this track is hint>");
	}
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

void Mp4Box::DumpBoxContent()
{
	char bufName[5] = {};
	for (auto p : m_HandlerList)
	{
		memcpy(bufName, &boxHeader.uBoxType, 4);
		if (p->m_strName == string(bufName))
		{
			p->DumpInfo(*this);
		}
	}
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
		COUNTTAB(box.dwLevel);
		DOLOG(" " + buftype + " :" + box.dwLevel + ", length:" + (box.boxHeader.uBoxSize64 - box.boxHeader.uHeaderLen) + ", Current Position:" + dwTotoal);
		box.DumpBoxContent();
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
		DOLOG(" " + buftype + " :" + box.dwLevel + ", length:" + (box.boxHeader.uBoxSize64 - box.boxHeader.uHeaderLen) + ", Current Position:" + dwTotoal);
		box.DumpSubBox();
	}

	DOLOG("---------- End Dump -------------------");
	DOLOG("");
}