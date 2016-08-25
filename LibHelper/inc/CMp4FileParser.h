////////////////////////////////////////////////////
// implement the parse mp4 format
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////
#pragma once
#include <string>
#include <vector>
#include "Helper.h"

/**
 * Mp4 File compose from box.
 * all data is big-endian
 */
struct Mp4BoxHeaderReal
{
	Mp4BoxHeaderReal() :uBoxSize(0), uBoxType(0){}
	unsigned int uBoxSize;
	unsigned int uBoxType;
};

/**
 * when uBoxSize equal 1, then use this struct
 */
struct Mp4BoxHeaderRealEx
{
	Mp4BoxHeaderRealEx() :uBoxSize(0), uBoxType(0){}
	unsigned int uBoxSize;
	unsigned int uBoxType;
	unsigned __int64 uBoxSize64;
};

struct Mp4BoxHeader
{
	Mp4BoxHeader(const Mp4BoxHeaderReal& rBoxHeaderRealEx) :uBoxSize64(rBoxHeaderRealEx.uBoxSize), uBoxType(rBoxHeaderRealEx.uBoxType), uHeaderLen(0){}
	Mp4BoxHeader(const Mp4BoxHeaderRealEx& rBoxHeaderRealEx) :uBoxSize64(rBoxHeaderRealEx.uBoxSize64), uBoxType(rBoxHeaderRealEx.uBoxType), uHeaderLen(0){}
	Mp4BoxHeader() :uBoxSize64(0), uBoxType(0), uHeaderLen(0){}

	Mp4BoxHeader& operator=(const Mp4BoxHeaderReal& rBoxHeaderRealEx)
	{ 
		uBoxSize64 = rBoxHeaderRealEx.uBoxSize;
		uBoxType = rBoxHeaderRealEx.uBoxType;
	}

	Mp4BoxHeader& operator=(const Mp4BoxHeaderRealEx& rBoxHeaderRealEx)
	{
		uBoxSize64 = rBoxHeaderRealEx.uBoxSize64;
		uBoxType = rBoxHeaderRealEx.uBoxType;
	}

	unsigned __int64 uBoxSize64;
	unsigned int uBoxType;
	unsigned int uHeaderLen;
};

struct Mp4Box
{
	Mp4Box() : pBoxBody(0), dwLevel(0){}
	~Mp4Box(){ SAFE_DELETE(pBoxBody); }
	
	void DumpSubBox();
	void GetSubBox(Mp4Box& rBox, DWORD dwPosition);
	void GetSubBoxHeader(Mp4BoxHeader& rBoxHeader, DWORD dwPosition);
	bool Is64Size();
	bool IsHaveSub();

	Mp4BoxHeader boxHeader;
	char* pBoxBody;
	DWORD dwLevel;
	static std::vector<string> m_ContanerTypeList;

};

class CMp4FileParser
{
public:
	CMp4FileParser(){};
	virtual ~CMp4FileParser(){}

	void Load(std::wstring& mp4filePath);
	void GetBoxHeader(Mp4BoxHeader& rBoxHeader, BOOL &bIs64, DWORD dwPosition = 0);
	void GetBox(Mp4Box& rBox, DWORD dwPosition = 0);

	void DumpTopLevel();
	bool IsSucceed(){ return m_bRes; }
protected:
	std::vector<char> m_FileContent;	/**< will fill all file bytes in it */
	unsigned int m_bRes;				/**< all function's result is fill in it, true or false */
};
