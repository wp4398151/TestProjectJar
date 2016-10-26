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

	unsigned __int64 uBoxSize64;	// 大小是整个BOX的大小，包括Header
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

/**
 * Movie Header 媒体的总体信息，主要包括时长，以及帧率
 */
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
 * Chunk Offset, 定义trunk在媒体流中的偏移
 */
class BoxSTCOHandler : public BoxHandlerBase
{
public:
	struct BoxStructHeader{
		DWORD dwVersionAndFlag;
		DWORD dwEntriesCount;
	};
	struct BoxBody{
		DWORD dwChunkOffset;	/**< the offset is relative mdat body */
	};
	BoxSTCOHandler() : BoxHandlerBase("stco"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
 * Sample Size，定义了每个Sample的大小
 */
class BoxSTSZHandler : public BoxHandlerBase
{
public:
	struct BoxStructHeader{
		DWORD dwVersionAndFlag;
		DWORD dwSampleSize;			/**< enable if the sample size same. other wise 0, see Entries Count*/
		DWORD dwEntriesCount;		/**< Count of entries */
	};
	struct BoxBody{
		DWORD dwSampleSize;
	};
	BoxSTSZHandler(): BoxHandlerBase("stsz"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
* Data Reference， 获取媒体数据的引用信息
*/
class BoxDREFHandler : public BoxHandlerBase
{
public:
	struct BoxStructHeader{
		DWORD dwVersionAndFlag;
		DWORD dwEntryCount;
	};
	struct BoxBody{
		DWORD dwRefSize;				
		DWORD dwRefType;			/**< count of sample in every chunk */
		DWORD dwVersionAndFlag;			/**< the description of every sample reference to stsd box */
	};
	BoxDREFHandler() : BoxHandlerBase("dref"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
 * Sound Media Head, 包含了声音控制信息
 */
class BoxSMHDHandler : public BoxHandlerBase
{
public:
	struct BoxStruct
	{
		DWORD dwVersionAndFlag;
		WORD wEqualization;			/**< Sound mix Equalization */
		WORD wReserved;
	};

	BoxSMHDHandler() : BoxHandlerBase("smhd"){}
	virtual void DumpInfo(Mp4Box& rBox);
};

/**
 * Video Media informationm header, 视频媒体控制信息
 */
class BoxVMHDHandler :public BoxHandlerBase
{
public:	
	struct BoxStruct
	{
		DWORD dwVersionAndFlag;		
		WORD wGraphicMode;			/**< switch for quick Draw */
		CHAR opColor[6];			/**< specify the red, green, blue color for transfer */
	};

	BoxVMHDHandler() : BoxHandlerBase("vmhd"){}
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
 * STSD 中的DescriptionBox
 */
class SampleDescriptionBox
{
public:
	struct SampleDescriptionBoxHeader
	{
		DWORD dwSize;
		DWORD dwType;
		CHAR reserve[6];
		USHORT usReference;
	};
	
	SampleDescriptionBoxHeader m_Header;
	DWORD m_dwLevel;

	static SampleDescriptionBox* Dump( char* pBodyBytes, DWORD dwLevel);
	virtual bool SetBody(char* pBodyBytes) = 0;
};

class MP4ASampleDescriptionBox : public SampleDescriptionBox
{
public:
#pragma pack(push, 1)
	struct SampleDescriptionBoxBody
	{
		WORD usEncodeVersion;	// encode version
		WORD usReserved;		// revision level
		DWORD dwEncodeVendor;
		WORD wChannels;
		WORD wSampleSize;
		WORD wAudioCompressionId;
		WORD wAudioPacketSize;
		DWORD dwSampleRate;
	};
#pragma pack(pop)

public:
	virtual bool SetBody(char* pBodyBytes);

	SampleDescriptionBoxBody m_Body;
};

class AVC1SampleDescriptionBox : public SampleDescriptionBox
{
public:
#pragma pack(push, 1)
	struct SampleDescriptionBoxBody
	{
		WORD usEncodeVersion;	// encode version
		WORD usReserved;		// revision level
		DWORD dwEncodeVendor;	
		DWORD dwTemporalQuality;
		DWORD dwSpatialQuality; 	
		WORD usWidth;
		WORD usHeight;
		DWORD dwHoriz_DPI;
		DWORD dwVert_DPI;
		DWORD dwReserved;		// quicktime video data size
		WORD usFrameCount;
		BYTE bcompressionNameLen;
		CHAR compression_name[31];
		WORD usBitDepth;
		WORD usPredefine;		//quicktime video color table id
	};
#pragma pack(pop)
public:
	virtual bool SetBody(char* pBodyBytes);

	SampleDescriptionBoxBody m_Body;
};

/**
* ESDS Element Stream Descriptors, 存放音频aac的具体config
*/
class BoxESDSHandler : public BoxHandlerBase
{
public:
#pragma pack(push, 1)
	struct BoxStructHeader
	{
		DWORD dwVersionAndFlag;
		BYTE cESDSType;	// just support type 3
	};

	struct BoxBody
	{
		BYTE wSize;		// sps size 
		CHAR* pContent;	// sps content
	};
#pragma pack (pop)

	BoxESDSHandler() : BoxHandlerBase("esds"){}
	virtual void DumpInfo(Mp4Box& rBox);
};


/**
* AVCC 保存SPS和PPS,用来辅助解码数据
*/
class BoxAVCCHandler : public BoxHandlerBase
{
public:
#pragma pack(push, 1)
	struct BoxStructHeader
	{
		BYTE version;
		BYTE profileID;
		BYTE h246CompatibleProfile;
		BYTE h264Level;
		BYTE Reserved;
	};
	
	struct BoxBody
	{
		BYTE spsCount;		// sps count
		WORD spsSize;		// sps size 
		CHAR* spsContent;	// sps content
		BYTE ppsCount;		// pps count
		WORD ppsSize;		// pps size 
		CHAR* ppsContent;	// pps content
	};
#pragma pack (pop)

	BoxAVCCHandler() : BoxHandlerBase("avcc"){}
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
	static bool GetBox(Mp4Box &rBox, char* pData, DWORD uLevel);
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
