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

// �����ѵļ����ò����ʵ�ֵ�RGB24 toYUV240���㷨
// ������RGBA32 to YUV240�Ĳ���
// Conversion from RGB24 to YUV420
// YUV420 �Ĵ�СΪw*h*1.5
void InitLookupTable();
int ConvertRGBtoYUV(int w, int h, unsigned char *bmp, unsigned int *yuv, bool bIsRGBA);
// Conversion from YUV420 to RGB24
void InitConvertTable();
void ConvertYUVtoRGB(unsigned char *src0, unsigned char *src1, unsigned char *src2, unsigned char *dst_ori,
	int width, int height);

struct EncoderParam
{
	int picWidth;
	int picHeight;
};

EncoderParam encoderParam;

char* CH264Encoder::convertRGB2YUV420(char* pRGBBits)	// ��RGB������ת����YUV420��x264�����ָ�ʽ����
{
	return NULL;
}

bool CH264Encoder::Init(){
	return true;
}

bool CH264Encoder::TestEncodeScreen()
{
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
	
	pX264Param->rc.i_bitrate = 1024*30;	// �������������£�Խ��Խ�ã�����������֡
	pX264Param->i_fps_den = 1;			//֡�ʷ�ĸ  
	pX264Param->i_fps_num = 10;			//֡�ʷ���  
	// ֡����ʱ��
	pX264Param->i_timebase_den = pX264Param->i_fps_num;
	pX264Param->i_timebase_num = pX264Param->i_fps_den;
	
	res = x264_param_apply_profile(pX264Param, x264_profile_names[0]);
	assert(res == 0);

	InitLookupTable();	// ��ʼ��RGB��YUV�ı�

	x264_t* pX264Handle = NULL;
	pX264Handle = x264_encoder_open(pX264Param);
	x264_encoder_parameters(pX264Handle, pX264Param);
	x264_nal_t *pNals = NULL;
	int iNal = 0;
	res = x264_encoder_headers(pX264Handle, &pNals, &iNal);
	
	int totalFrame = 200;
	//NAL_SPS
	x264_picture_t pic_in;
	x264_picture_t pic_out;
	res = x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);
	assert(res == 0);

	x264_picture_init(&pic_out);
	assert(res == 0);

	FILE* pFile = fopen(targetPath.c_str(), "wb");
	assert(pFile);
	//------------------------------ 
	// ��֯��ǰ������RGBAת����YUV420
	size_t yuv_size = (width * height * 4 * 3)/2;
	uint8_t* pBuf = (uint8_t*)malloc(yuv_size);
	uint8_t* pRealBuf = (uint8_t*)malloc(yuv_size);
	
	int picWidth = 0;
	int picHeight = 0;
	int pixelBitSize = 0;
	// ��ǰ���ܻ�������֡��
	int iMaxFrames = x264_encoder_maximum_delayed_frames(pX264Handle);

	int yuv420ByteCount = (width * height * 3 / 2);
	for (int i = 0; i < totalFrame; ++i)
	{
		// ��ȡ��ǰ�����RGBA
		char* pRBG = GetCaptureScreenDCRGBbits(picWidth, picHeight, pixelBitSize);
		res = ConvertRGBtoYUV(picWidth, picHeight, (unsigned char*)pRBG, (unsigned int*)pBuf, true);

		
	 	char* pNewBuf = RgbaToYUV420(pRBG, picWidth, picHeight);
	
		for (int j = 0; j < yuv420ByteCount ; ++j)
		{
			pRealBuf[j] = pBuf[j];
		}

		assert(res);
		pic_in.img.i_csp = X264_CSP_I420;
		pic_in.img.plane[0] = pRealBuf;
		pic_in.img.plane[1] = pic_in.img.plane[0] + width * height;
		pic_in.img.plane[2] = pic_in.img.plane[1] + width * height / 4;
		pic_in.i_pts = i;

		x264_nal_t *pNals = NULL;
		int iNal = 0;
		res = x264_encoder_encode(pX264Handle, &pNals, &iNal, &pic_in, &pic_out);
		if (res > 0)			// �ɹ���ñ�������
		{
			DOLOG("����ɹ�");
			for (int nalId = 0; nalId < iNal ;++nalId)
			{
				ASSERT_EQU(fwrite(pNals[nalId].p_payload, 1, pNals[nalId].i_payload, pFile), pNals[nalId].i_payload);
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
		SAFE_FREE(pRBG);
	}
	
	fclose(pFile);
	x264_picture_clean(&pic_out);
	x264_picture_clean(&pic_in);
	x264_encoder_close(pX264Handle);
	SAFE_DELETE(pX264Param);
	return true;
}

// Conversion from RGB to YUV420
int RGB2YUV_YR[256], RGB2YUV_YG[256], RGB2YUV_YB[256];
int RGB2YUV_UR[256], RGB2YUV_UG[256], RGB2YUV_UBVR[256];
int RGB2YUV_VG[256], RGB2YUV_VB[256];
// Conversion from YUV420 to RGB24
static long int crv_tab[256];
static long int cbu_tab[256];
static long int cgu_tab[256];
static long int cgv_tab[256];
static long int tab_76309[256];
static unsigned char clp[1024]; //for clip in CCIR601
//
// Table used for RGB to YUV420 conversion
//
void InitLookupTable()
{
	int i;
	for (i = 0; i < 256; i++) RGB2YUV_YR[i] = (float)65.481 * (i << 8);
	for (i = 0; i < 256; i++) RGB2YUV_YG[i] = (float)128.553 * (i << 8);
	for (i = 0; i < 256; i++) RGB2YUV_YB[i] = (float)24.966 * (i << 8);
	for (i = 0; i < 256; i++) RGB2YUV_UR[i] = (float)37.797 * (i << 8);
	for (i = 0; i < 256; i++) RGB2YUV_UG[i] = (float)74.203 * (i << 8);
	for (i = 0; i < 256; i++) RGB2YUV_VG[i] = (float)93.786 * (i << 8);
	for (i = 0; i < 256; i++) RGB2YUV_VB[i] = (float)18.214 * (i << 8);
	for (i = 0; i < 256; i++) RGB2YUV_UBVR[i] = (float)112 * (i << 8);
}

//
// Convert from RGB24 to YUV420
// ����yuv420�ĸ�ʽ��Ӧ�÷���Ĵ�С��w*h*1.5
//
int ConvertRGBtoYUV(int w, int h, unsigned char *bmp, unsigned int *yuv, bool bIsRGBA)
{
	unsigned int *u, *v, *y, *uu, *vv;
	unsigned int *pu1, *pu2, *pu3, *pu4;
	unsigned int *pv1, *pv2, *pv3, *pv4;
	unsigned char *r, *g, *b;
	int i, j;
	uu = new unsigned int[w*h];
	vv = new unsigned int[w*h];
	if (uu == NULL || vv == NULL)
		return 0;
	y = yuv;
	u = uu;
	v = vv;

	// Get r,g,b pointers from bmp image data....
	r = bmp;
	g = bmp + 1;
	b = bmp + 2;
	int stepCount = bIsRGBA?4:3;
	//Get YUV values for rgb values...
	for (i = 0; i<h; i++)
	{
		for (j = 0; j<w; j++)
		{
			*(y++) = (RGB2YUV_YR[*r] + RGB2YUV_YG[*g] + RGB2YUV_YB[*b] + 1048576) >> 16;
			*(u++) = (-RGB2YUV_UR[*r] - RGB2YUV_UG[*g] + RGB2YUV_UBVR[*b] + 8388608) >> 16;
			*(v++) = (RGB2YUV_UBVR[*r] - RGB2YUV_VG[*g] - RGB2YUV_VB[*b] + 8388608) >> 16;
			r += stepCount;
			g += stepCount;
			b += stepCount;
		}
	}
	// Now sample the U & V to obtain YUV 4:2:0 format
	// Sampling mechanism...
	/* @ -> Y
	# -> U or V
	@ @ @ @
	# #
	@ @ @ @
	@ @ @ @
	# #
	@ @ @ @
	*/
	// Get the right pointers...
	u = yuv + w*h;
	v = u + (w*h) / 4;
	// For U
	pu1 = uu;
	pu2 = pu1 + 1;
	pu3 = pu1 + w;
	pu4 = pu3 + 1;

	// For V
	pv1 = vv;
	pv2 = pv1 + 1;
	pv3 = pv1 + w;
	pv4 = pv3 + 1;

	// Do sampling....
	for (i = 0; i<h; i += 2)
	{
		for (j = 0; j<w; j += 2)
		{
			*(u++) = (*pu1 + *pu2 + *pu3 + *pu4) >> 2;
			*(v++) = (*pv1 + *pv2 + *pv3 + *pv4) >> 2;
			pu1 += 2;
			pu2 += 2;
			pu3 += 2;
			pu4 += 2;
			pv1 += 2;
			pv2 += 2;
			pv3 += 2;
			pv4 += 2;
		}

		pu1 += w;
		pu2 += w;
		pu3 += w;
		pu4 += w;
		pv1 += w;
		pv2 += w;
		pv3 += w;
		pv4 += w;
	}

	delete uu;
	delete vv;
	return 1;
}

//
//Initialize conversion table for YUV420 to RGB
//
void InitConvertTable()
{
	long int crv, cbu, cgu, cgv;
	int i, ind;
	crv = 104597; cbu = 132201; /* fra matrise i global.h */
	cgu = 25675; cgv = 53279;
	for (i = 0; i < 256; i++) {
		crv_tab[i] = (i - 128) * crv;
		cbu_tab[i] = (i - 128) * cbu;
		cgu_tab[i] = (i - 128) * cgu;
		cgv_tab[i] = (i - 128) * cgv;
		tab_76309[i] = 76309 * (i - 16);
	}

	for (i = 0; i<384; i++)
		clp[i] = 0;
	ind = 384;
	for (i = 0; i<256; i++)
		clp[ind++] = i;
	ind = 640;
	for (i = 0; i<384; i++)
		clp[ind++] = 255;
}

//
// Convert from YUV420 to RGB24
//
void ConvertYUVtoRGB(unsigned char *src0, unsigned char *src1, unsigned char *src2, unsigned char *dst_ori,
	int width, int height)
{
	int y1, y2, u, v;
	unsigned char *py1, *py2;
	int i, j, c1, c2, c3, c4;
	unsigned char *d1, *d2;

	py1 = src0;
	py2 = py1 + width;
	d1 = dst_ori;
	d2 = d1 + 3 * width;

	for (j = 0; j < height; j += 2) {
		for (i = 0; i < width; i += 2) {
			u = *src1++;
			v = *src2++;
			c1 = crv_tab[v];
			c2 = cgu_tab[u];
			c3 = cgv_tab[v];
			c4 = cbu_tab[u];

			//up-left
			y1 = tab_76309[*py1++];
			*d1++ = clp[384 + ((y1 + c1) >> 16)];
			*d1++ = clp[384 + ((y1 - c2 - c3) >> 16)];
			*d1++ = clp[384 + ((y1 + c4) >> 16)];

			//down-left
			y2 = tab_76309[*py2++];
			*d2++ = clp[384 + ((y2 + c1) >> 16)];
			*d2++ = clp[384 + ((y2 - c2 - c3) >> 16)];
			*d2++ = clp[384 + ((y2 + c4) >> 16)];

			//up-right
			y1 = tab_76309[*py1++];
			*d1++ = clp[384 + ((y1 + c1) >> 16)];
			*d1++ = clp[384 + ((y1 - c2 - c3) >> 16)];
			*d1++ = clp[384 + ((y1 + c4) >> 16)];

			//down-right
			y2 = tab_76309[*py2++];
			*d2++ = clp[384 + ((y2 + c1) >> 16)];
			*d2++ = clp[384 + ((y2 - c2 - c3) >> 16)];
			*d2++ = clp[384 + ((y2 + c4) >> 16)];
		}
		d1 += 3 * width;
		d2 += 3 * width;
		py1 += width;
		py2 += width;
	}
}
////////////////////////////////////////
// return the buffer of yuv420 on success, return NULL when error occur.
// return buffer mast free
char* CH264Encoder::RgbaToYUV420(char* pRgbaBuf, int width, int height)
{
	DOLOG("witdh:" + width + ", height:" + height);
	int yuvSize = width*height * 3 / 2;
	char* pYBuf = NULL;
	char* pUBuf = NULL;
	char *pVBuf = NULL;
	char* pYUVBuf = NULL;

	pYUVBuf = (char*)malloc(yuvSize);
	memset(pYUVBuf, 0, yuvSize);

	//yuv��ʽ��YUV���� 
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
			/*y = (unsigned char)(  0.299* r + 0.587* g +  0.114 * b )   ;
			u = (unsigned char)( -0.169 * r -  0.332 * g + 0.500 * b + 128)  ;
			v = (unsigned char)( 0.500 * r +0.419 * g -  0.0813 * b + 128);  */

			//�������ȷ�Ĺ�ʽ�������� 
			y = (UCHAR)(0.257*r + 0.504*g + 0.098*b + 16);
			u = (UCHAR)(-0.148*r - 0.291*g + 0.439*b + 128);
			v = (UCHAR)(0.439*r - 0.368*g - 0.071*b + 128);

			/*y=0.299*r + 0.587*g + 0.114*b;
			u=(r-y)*0.713 + 128;
			v=(b-y)*0.564 + 128;*/

			if (y>255)y = 255;
			else if (y < 0)y = 0;
			if (u>255)u = 255;
			else if (u < 0)u = 0;
			if (v>255)v = 255;
			else if (v < 0)v = 0;
			*(pYBuf++) = y;
			/**(pVBuf++)=v;
			*(pUBuf++)=u;*/
			pRgbaBuf += 4;
			/**(pRgbBuf++)=y;
			*(pRgbBuf++)=v;
			*(pRgbBuf++)=u;*/

			if (i % 2 == 0 && j % 2 == 0)
			{
				//��uvȡ�� 
				*(pVBuf++) = v;
				*(pUBuf++) = u;
			}
		}
	}

	//pImage->imageData=pYUVBuf; 
	/*IplImage* yimg = cvCreateImageHeader(cvSize(width, height),IPL_DEPTH_8U,1);
	cvSetData(yimg,pYUVBuf,width);*/
	//pImage->imageSize=yuvSize; 
	//fprintf(pFile,"%s",pYUVBuf); 
	//fwrite(pYUVBuf, 1, yuvSize, pFile);
	//pImage->imageData=pYBuf; 
	//pImage->imageDataOrigin=pYBuf; 
	/*strcpy_s(pImage->channelSeq,"YVU");
	pImage->dataOrder=1;*/

	return pYUVBuf;
}
