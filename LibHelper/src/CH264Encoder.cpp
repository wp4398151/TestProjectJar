////////////////////////////////////////////////////
// 常用帮助函数
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////
#include "CH264Encoder.h"
#include <stdint.h>
#include "x264.h"
#include <assert.h>

// 网上搜的几个用查表来实现的RGB24 toYUV240的算法
// 增加了RGBA32 to YUV240的参数
// Conversion from RGB24 to YUV420
// YUV420 的大小为w*h*1.5
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

char* CH264Encoder::convertRGB2YUV420(char* pRGBBits)	// 将RGB的内容转换成YUV420，x264以这种格式输入
{
	return NULL;
}

bool CH264Encoder::Init()
{
	int res = 0;
	x264_t* pX264Handle = NULL;
	x264_param_t* pX264Param = new x264_param_t;
	assert(pX264Param!=NULL);
	
	res = x264_param_default_preset(pX264Param,
					"veryfast",			// 编码速度模式
					"zerolatency");		// 即时编码不会产生缓存帧
	assert(res == 0);

	pX264Param->i_threads = X264_SYNC_LOOKAHEAD_AUTO;	// 自动线程同步
	pX264Param->i_width = encoderParam.picWidth;
	pX264Param->i_height= encoderParam.picHeight;
	pX264Param->i_frame_total = 0;	// 录制非定长视频不需要知道帧数
	pX264Param->i_keyint_max = 30;	// 设置IDR帧(完整画面数据帧)允许的最大间隔帧数
	
	pX264Param->i_bframe = 5;	// 允许参考帧之间允许存在多少B帧
	pX264Param->b_open_gop = 0;
	pX264Param->i_bframe_pyramid = 2;
	pX264Param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
	
	pX264Param->i_log_level = X264_LOG_DEBUG;// 需要启用日志
	
	pX264Param->rc.i_bitrate = 1024*30;	// 环境允许的情况下，越大越好，否则会出现跳帧
	pX264Param->i_fps_den = 1;			//帧率分母  
	pX264Param->i_fps_num = 10;			//帧率分子  
	// 帧间间隔时间
	pX264Param->i_timebase_den = pX264Param->i_fps_num;
	pX264Param->i_timebase_num = pX264Param->i_fps_den;
	
	res = x264_param_apply_profile(pX264Param, x264_profile_names[0]);
	assert(res == 0);

	return false;
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
// 根据yuv420的格式，应该分配的大小是w*h*1.5
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
			*y++ = (RGB2YUV_YR[*r] + RGB2YUV_YG[*g] + RGB2YUV_YB[*b] + 1048576) >> 16;
			*u++ = (-RGB2YUV_UR[*r] - RGB2YUV_UG[*g] + RGB2YUV_UBVR[*b] + 8388608) >> 16;
			*v++ = (RGB2YUV_UBVR[*r] - RGB2YUV_VG[*g] - RGB2YUV_VB[*b] + 8388608) >> 16;
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
			*u++ = (*pu1 + *pu2 + *pu3 + *pu4) >> 2;
			*v++ = (*pv1 + *pv2 + *pv3 + *pv4) >> 2;
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