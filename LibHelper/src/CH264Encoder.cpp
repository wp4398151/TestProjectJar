////////////////////////////////////////////////////
// ���ð�������
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////
#include "CH264Encoder.h"
#include <stdint.h>
extern "C"
{
	#include "x264.h"
}
#include <assert.h>

struct EncoderParam
{
	int picWidth;
	int picHeight;
};

EncoderParam encoderParam;

bool CH264Encoder::Init(){
	return true;
}

bool CH264Encoder::TestEncodeScreenUseX264Encoder()
{
	string targetPath;
	GetAppPathA(targetPath);
	targetPath += "\\test.h264";
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	encoderParam.picWidth = width;
	encoderParam.picHeight = height;

	int res = 0;

	FILE* pFile = fopen(targetPath.c_str(), "wb");
	assert(pFile);
	//------------------------------ 
	// ��֯��ǰ������RGBAת����YUV420
	size_t yuv_size = (width * height * 4 * 3) / 2;

	int pixelBitSize = 0;
	// ��ǰ���ܻ�������֡��
	int yuv420ByteCount = (width * height * 3 / 2);
	char* pYUVBuf = (char*)malloc(yuv420ByteCount);
	char* pRBGA = (char*)malloc(width*height * 4);
	assert(pYUVBuf);
	int totalFrame = 100;
	INT startCount = GetTickCount();

	VideoEncoder *pEncoder = CreateX264Encoder(30, width, height, 5, 60000, 60000, false);

	x264_picture_t pic_in;
	res = x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);
	
	HighQualityResolutionTimeLite timeLite;
	int picWidth = 0; 
	int picHeight = 0;
	for (int i = 0; i < totalFrame; ++i)
	{
		timeLite.Reset();
		DOLOG("---------------------------------------------\r\n");
		// ��ȡ��ǰ�����RGBA
		res = GetCaptureScreenDCRGBAbitsEx(picWidth, picHeight, pixelBitSize, pRBGA);
		ASSERT_TRUE(res);

		DOLOG(">> Capture Use :" + timeLite.GetTimelapse() + "ms \r\n");
		timeLite.Reset();

		res = ConvertRBGA2YUV420Ex(pRBGA, picWidth, picHeight, pYUVBuf);
		ASSERT_TRUE(res);

		DOLOG(">> Counvert Use :" + timeLite.GetTimelapse() + "ms \r\n");
		timeLite.Reset();

		pic_in.img.i_csp = X264_CSP_I420;
		pic_in.img.plane[0] = (uint8_t*)pYUVBuf;
		pic_in.img.plane[1] = pic_in.img.plane[0] + width * height;
		pic_in.img.plane[2] = pic_in.img.plane[1] + width * height / 4;
		//pic_in.i_pts = i*30;

		x264_nal_t *pNals = NULL;
		int iNal = 0;
		res = pEncoder->Encode(&pic_in, pNals, &iNal);

		if (res)			// �ɹ���ñ�������
		{
			DOLOG(">> Encode Use :" + timeLite.GetTimelapse() + "ms \r\n");
			timeLite.Reset();
			for (int nalId = 0; nalId < iNal; ++nalId)
			{
				res = fwrite(pNals[nalId].p_payload, 1, pNals[nalId].i_payload, pFile);
				assert(res > 0);
				DOLOG(">> Write Use :" + timeLite.GetTimelapse() + "ms \r\n");
				timeLite.Reset();
			}
		}
		else
		{
			DOLOG("�������");
		}
	}

	startCount = GetTickCount() - startCount;
	DOLOG("Total " + totalFrame + " Lapse : " + startCount + "mili sec \r\n");

	delete pEncoder;
	fclose(pFile);
	SAFE_FREE(pRBGA);
	SAFE_FREE(pYUVBuf);
	return true;
}

bool CH264Encoder::TestEncodeScreen()
{
// Key param-------------------------------------------------------------
	//int fps;						// ����֡����ֱ��Ӱ��¼�Ƴ�����ͼ���С
	//int quality;					// ��������1��8�� Խ��ÿ֡����Խ�á�ͬ����СԽ�� 
	//CTSTR preset,					// ��֮������
	//bool bUse444,					// �Ƿ�ʹ��444����
	//ColorDescription &colorDesc,  // ɫ������
	//int maxBitrate,				// ��������	
	//int bufferSize,				// �������Ĵ�С
	//bool bUseCFR					// �Ƿ�ʹ�ù̶�֡��
	//bool bUseCBR					// �Ƿ�ʹ�ù̶�����
//----------------------------------------------------------------------------
	string targetPath;
	GetAppPathA(targetPath);
	targetPath += "\\test.h264";
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	encoderParam.picWidth = width;
	encoderParam.picHeight = height;

	int res = 0;
	x264_param_t* pX264Param = new x264_param_t;

	assert(pX264Param!=NULL);
	
	res = x264_param_default_preset(pX264Param,
					"veryfast",			// �����ٶ�ģʽ
					"zerolatency");		// ��ʱ���벻���������֡
	assert(res == 0);

	pX264Param->i_threads = X264_SYNC_LOOKAHEAD_AUTO;	// �Զ��߳�ͬ��
	pX264Param->i_width = encoderParam.picWidth;
	pX264Param->i_height= encoderParam.picHeight;
	pX264Param->i_frame_total = 0;	// ¼�ƷǶ�����Ƶ����Ҫ֪��֡��
	pX264Param->i_keyint_max = 30;	// ����IDR֡(������������֡)����������֡��
	
	pX264Param->i_bframe = 5;	// ����ο�֮֡��������ڶ���B֡
	pX264Param->b_open_gop = 0;
	pX264Param->i_bframe_pyramid = 2;
	pX264Param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
	
	pX264Param->i_log_level = X264_LOG_DEBUG;// ��Ҫ������־

	pX264Param->b_vfr_input = 0;
	pX264Param->rc.i_bitrate = 1024*30;	// �������������£�Խ��Խ�ã�����������֡
	pX264Param->i_fps_den = 1;			//֡�ʷ�ĸ  
	pX264Param->i_fps_num = 30;			//֡�ʷ���  
	// ֡����ʱ��
	pX264Param->i_timebase_den = pX264Param->i_fps_num;
	pX264Param->i_timebase_num = pX264Param->i_fps_den;
	
	res = x264_param_apply_profile(pX264Param, x264_profile_names[1]);
	assert(res == 0);

	x264_t* pX264Handle = NULL;
	pX264Handle = x264_encoder_open(pX264Param);
	x264_encoder_parameters(pX264Handle, pX264Param);
	x264_nal_t *pNals = NULL;
	int iNal = 0;
	res = x264_encoder_headers(pX264Handle, &pNals, &iNal);
	
	int totalFrame = 200;
	//NAL_SP
	x264_picture_t pic_in;
	x264_picture_t pic_out;
	res = x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);
	assert(res == 0);
	x264_picture_init(&pic_out);

	FILE* pFile = fopen(targetPath.c_str(), "wb");
	assert(pFile);
	//------------------------------ 
	// ��֯��ǰ������RGBAת����YUV420
	size_t yuv_size = (width * height * 4 * 3)/2;
	
	int picWidth = 0;
	int picHeight = 0;
	int pixelBitSize = 0;
	// ��ǰ���ܻ�������֡��
	int iMaxFrames = x264_encoder_maximum_delayed_frames(pX264Handle);
	int yuv420ByteCount = (width * height * 3 / 2);
	char* pYUVBuf= (char*)malloc(yuv420ByteCount);
	char* pRBGA = (char*)malloc(width*height*4);
	assert(pYUVBuf);

	INT startCount = GetTickCount();

	HighQualityResolutionTimeLite timeLite;
	for (int i = 0; i < totalFrame; ++i)
	{
		timeLite.Reset();
		DOLOG("---------------------------------------------\r\n");
		// ��ȡ��ǰ�����RGBA
		res = GetCaptureScreenDCRGBAbitsEx(picWidth, picHeight, pixelBitSize, pRBGA);
		ASSERT_TRUE(res);

		DOLOG(">> Capture Use :"+timeLite.GetTimelapse()+"ms \r\n");
		timeLite.Reset();

		res = ConvertRBGA2YUV420Ex(pRBGA, picWidth, picHeight, pYUVBuf);
		ASSERT_TRUE(res);

		DOLOG(">> Counvert Use :"+timeLite.GetTimelapse()+"ms \r\n");
		timeLite.Reset();

		pic_in.img.i_csp = X264_CSP_I420;
		pic_in.img.plane[0] = (uint8_t*)pYUVBuf;
		pic_in.img.plane[1] = pic_in.img.plane[0] + width * height;
		pic_in.img.plane[2] = pic_in.img.plane[1] + width * height / 4;
		//pic_in.i_pts = i*30;

		x264_nal_t *pNals = NULL;
		int iNal = 0;
		res = x264_encoder_encode(pX264Handle, &pNals, &iNal, &pic_in, &pic_out);
		if (res > 0)			// �ɹ���ñ�������
		{
			DOLOG(">> Encode Use :"+timeLite.GetTimelapse()+"ms \r\n");
			timeLite.Reset();
			for (int nalId = 0; nalId < iNal ;++nalId)
			{
				res = fwrite(pNals[nalId].p_payload, 1, pNals[nalId].i_payload, pFile);
				assert(res > 0);
				DOLOG(">> Write Use :"+timeLite.GetTimelapse()+"ms \r\n");
				timeLite.Reset();
			}
		}
		else if (res == 0 )		// ����ɹ����������ݱ�����
		{
			DOLOG("���������ݱ�����");
		}
		else// < 0 Ϊ����ʧ��
		{
			DOLOG("�������");
		}
		
		int countCacheFrame = x264_encoder_delayed_frames(pX264Handle);
		DOLOG("��ǰ�������֡��Ϊ" + countCacheFrame );
		if (countCacheFrame>0)
		{
			res = x264_encoder_encode(pX264Handle, &pNals, &iNal, NULL, &pic_out);
			if (res > 0)
			{
				timeLite.Reset();
				for (int nalId = 0; nalId < iNal ;++nalId)
				{
					res = fwrite(pNals[nalId].p_payload, 1, pNals[nalId].i_payload, pFile);
					assert(res > 0);
					DOLOG(">> Write Use :"+timeLite.GetTimelapse()+"ms \r\n");
					timeLite.Reset();
				}
			}
			else if(res == 0)		// ����ɹ����������ݱ�����
			{
				DOLOG("���������ݱ�����");
			}
			else// < 0 Ϊ����ʧ��
			{
				DOLOG("�������");
			}
		}
	}
	
	startCount = GetTickCount() - startCount;
	DOLOG("Total " + totalFrame + " Lapse : " + startCount + "mili sec \r\n");

	fclose(pFile);
	SAFE_FREE(pRBGA);
	SAFE_FREE(pYUVBuf);
	//x264_picture_clean(&pic_in);
	x264_encoder_close(pX264Handle);
	SAFE_DELETE(pX264Param);
	return true;
}

////////////////////////////////////////
// return the buffer of yuv420 on success, return NULL when error occur.
// return buffer provide by user, length is (width*height*3/2)
BOOL CH264Encoder::ConvertRBGA2YUV420Ex(char* pRgbaBuf, int width, int height, char* pYUVBuf)
{
	ASSERT_NOTNULLRET(pRgbaBuf, FALSE);
	ASSERT_NOTNULLRET(pYUVBuf, FALSE);

	DOLOG("witdh:" + width + ", height:" + height);
	int yuvSize = width*height * 3 / 2;
	char* pYBuf = NULL;
	char* pUBuf = NULL;
	char *pVBuf = NULL;
	pYBuf = pYUVBuf;
	pUBuf = pYBuf + width*height;
	pVBuf = pUBuf + width*height / 4;

	unsigned char r, g, b;
	unsigned char y, u, v;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			b = *(pRgbaBuf);
			g = *(pRgbaBuf + 1);
			r = *(pRgbaBuf + 2);

			y = (UCHAR)(0.257*r + 0.504*g + 0.098*b + 16);
			u = (UCHAR)(-0.148*r - 0.291*g + 0.439*b + 128);
			v = (UCHAR)(0.439*r - 0.368*g - 0.071*b + 128);

			if (y>255)y = 255;
			else if (y < 0)y = 0;
			if (u>255)u = 255;
			else if (u < 0)u = 0;
			if (v>255)v = 255;
			else if (v < 0)v = 0;
			*(pYBuf++) = y;
			pRgbaBuf += 4;
			if (i % 2 == 0 && j % 2 == 0)
			{
				*(pVBuf++) = v;
				*(pUBuf++) = u;
			}
		}
	}

	return TRUE;
}

////////////////////////////////////////
// return the buffer of yuv420 on success, return NULL when error occur.
// return buffer mast free
char* CH264Encoder::ConvertRBGA2YUV420(char* pRgbaBuf, int width, int height)
{
	DOLOG("witdh:" + width + ", height:" + height);
	int yuvSize = width*height * 3 / 2;
	char* pYBuf = NULL;
	char* pUBuf = NULL;
	char *pVBuf = NULL;
	char* pYUVBuf = NULL;

	pYUVBuf = (char*)malloc(yuvSize);
	memset(pYUVBuf, 0, yuvSize);

	pYBuf = pYUVBuf;
	pUBuf = pYBuf + width*height;
	pVBuf = pUBuf + width*height / 4;

	unsigned char r, g, b;
	unsigned char y, u, v;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			b = *(pRgbaBuf);
			g = *(pRgbaBuf + 1);
			r = *(pRgbaBuf + 2);

			y = (UCHAR)(0.257*r + 0.504*g + 0.098*b + 16);
			u = (UCHAR)(-0.148*r - 0.291*g + 0.439*b + 128);
			v = (UCHAR)(0.439*r - 0.368*g - 0.071*b + 128);

			if (y>255)y = 255;
			else if (y < 0)y = 0;
			if (u>255)u = 255;
			else if (u < 0)u = 0;
			if (v>255)v = 255;
			else if (v < 0)v = 0;
			*(pYBuf++) = y;
			pRgbaBuf += 4;
			if (i % 2 == 0 && j % 2 == 0)
			{
				*(pVBuf++) = v;
				*(pUBuf++) = u;
			}
		}
	}

	return pYUVBuf;
}

