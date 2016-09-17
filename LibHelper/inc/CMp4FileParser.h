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

struct Mp4Box;
/**
* use handle the specific box content.
*/
class BoxHandlerBase
{
public:
	BoxHandlerBase(LPCSTR lpStr) : m_strName(lpStr){ }
	virtual void DumpInfo(Mp4Box&){};
public:
	bool operator==(const BoxHandlerBase& rBoxHandlerBase){ return rBoxHandlerBase.m_strName == m_strName; }
public:
	string m_strName;
};

class BoxMVHDHanlder : public BoxHandlerBase
{
public:
	struct BoxStruct
	{
		DWORD dwVersionAndflag;
		DWORD dwCreateTime;					/**< the seconds relate to 1904-01-01  */
		DWORD dwModificationTime;			
		DWORD dwTimeScale;
		DWORD dwTimeDuration;				/**< time length of this track  */
		DWORD dwRate;						/**< rate play rate */
		WORD wVolume;
		CHAR reserved[10];
		CHAR matrix[36];
		CHAR preDefine[24];
		DWORD dwNextTrackId;
	};
	BoxMVHDHanlder() : BoxHandlerBase("mvhd"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
* use to handle Track Header Box(tkhd)
*/
class BoxTKHDHanlder : public BoxHandlerBase
{
public:
	struct BoxStruct
	{
		DWORD dwVersionAndflag;
		DWORD dwCreateTime;					/**< the seconds relate to 1904-01-01  */
		DWORD dwModificationTime;			
		DWORD dwTrackId;					/**< can't repeat, and can't be zero */
		DWORD dwPreserved1;
		DWORD dwTimeDuration;				/**< time length of this track  */
		CHAR dwPreserved2[8];
		WORD wLayer;				
		WORD wAlterGroup;				
		WORD wVolume;
		WORD wReserved;
		CHAR matrix[36];
		DWORD dwWidth;
		DWORD dwHeight;
	};

	BoxTKHDHanlder() : BoxHandlerBase("tkhd"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
 * use to Descript sample to chunck reflection relationship(stsc)
 * this box is contain the frame list(will compile stts, stsc, stsz, stss, stco)
 */
class BoxSTSCHanlder : public BoxHandlerBase
{
public:
	struct BoxStructHeader{
		DWORD dwVersionAndFlag;
		DWORD dwEntryCount;
	};
	struct BoxBody{
		DWORD dwFirstChunk;				/**< start chunk id */
		DWORD dwSamplePerChunk;			/**< count of sample in every chunk */
		DWORD dwSampleDescriptionIndex;	/**< the description of every sample reference to stsd box */
	};
	
	BoxSTSCHanlder () : BoxHandlerBase("stsc"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
 * Sync Sample Atoms, 用于确定media中的关键帧. 
 * 其中记录了sample的index, 被标记的sample即为关键帧.
 * 特殊的，如果没有标记任何一个关键帧，则每一帧都是关键帧
 */
class BoxSTSSHandler :public BoxHandlerBase
{
public:
	struct BoxStructHeader{
		DWORD dwVersionAndFlag;
		DWORD dwEntryCount;
	};
	struct BoxBody{
		DWORD dwKeyFrameIndex;
	};
	BoxSTSSHandler() : BoxHandlerBase("stss"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
 * Time to Sample Atom, 提供了sample的duration信息, 也就是具体sample的时间映射方法.
 * DT(n+1) = DT(n) + STTS(n)
 */
class BoxSTTSHandler : public BoxHandlerBase
{
public:
	struct BoxStructHeader{
		DWORD dwVersionAndFlag;
		DWORD dwEntryCount;
	};
	struct BoxBody{
		DWORD dwSampleCount;			/**< 表示此偏移有多少个相同的sample相同持续时间的sample, e.g. 如果是video, 每隔一sample就是一帧， */
		DWORD dwSampleDuration;			/**< 用draction/timescale 就等于了实际一个sample所持续的second */
	};
	BoxSTTSHandler() : BoxHandlerBase("stts"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
 * 利用STSD sample Description正确解码media sample
 */
class BoxSTSDHandler : public BoxHandlerBase
{
public:
	struct BoxStructHeader
	{
		DWORD dwVersionAndflag;
		DWORD dwEntryCount;
	};
	struct BoxBody{
		DWORD dwSize;
		DWORD dwFormat;
		CHAR dwReserve[6];
		USHORT dwReference[2];
	};
	
	BoxSTSDHandler() : BoxHandlerBase("stsd"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
 * 用来映射movie时间到此track的时间
 */
class BoxELSTHandler : public BoxHandlerBase
{
public:
	struct BoxStructHeader{
		DWORD dwVersionAndFlag;
		DWORD dwEntryCount;
	};
	
	struct BoxBody{
		DWORD dwDuration;
		DWORD dwTime;
		DWORD dwSpeed;
	};
	BoxELSTHandler() : BoxHandlerBase("elst"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
* use to handle Media Header Box(mdhd)
*/
class BoxMDHDHanlder : public BoxHandlerBase
{
public:
	struct BoxStruct
	{
		DWORD dwVersionAndflag;
		DWORD dwCreateTime;
		DWORD dwModificationTime;
		DWORD dwTimeScale;
		DWORD dwTimeDuration;				/**< time length of this track  */
		DWORD dwlanguageAndPreDefine;		/**< media language code */
	};

	BoxMDHDHanlder() : BoxHandlerBase("mdhd"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
* use to handle Reference Box(hdlr)
*/
class BoxHDLRHanlder : public BoxHandlerBase
{
public:
	struct BoxStruct
	{
		DWORD dwVersionAndflag;
		DWORD dwPredefined;
		DWORD dwHandlerType;
		CHAR preserved[12];
		CHAR *pName;
	};
public:
	BoxHDLRHanlder() :BoxHandlerBase("hdlr"){}
	virtual void DumpInfo(Mp4Box& rBox);
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
	void DumpBoxContent();

	Mp4BoxHeader boxHeader;
	char* pBoxBody;
	DWORD dwLevel;
	static std::vector<string> m_ContanerTypeList;
	static std::vector<BoxHandlerBase*> m_HandlerList;

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
