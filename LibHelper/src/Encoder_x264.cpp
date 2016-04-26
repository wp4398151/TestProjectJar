/********************************************************************************
 Copyright (C) 2012 Hugh Bailey <obs.jim@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
********************************************************************************/

#include <inttypes.h>
#include <ws2tcpip.h>
#include "CH264Encoder.h"
extern "C"
{
#include "x264.h"
}

void get_x264_log(void *param, int i_level, const char *psz, va_list argptr)
{
	DOLOG("x264: " + psz );
}

struct VideoPacket
{
    list<BYTE> Packet;
    inline void FreeData() {Packet.clear();}
};

const float baseCRF = 22.0f;

bool valid_x264_string(const string &str, const char **x264StringList)
{
    do
    {
		if (str.compare(*x264StringList))
		{
            return true;
		}
    } while (*++x264StringList != 0);

    return false;
}

class X264Encoder : public VideoEncoder
{
    x264_param_t paramData;
    x264_t *x264;

    x264_picture_t picOut;

    int cur_pts_time;
    x264_nal_t *pp_nal;
    int pi_nal;

    int fps_ms;

    bool bRequestKeyframe;

    UINT width, height;

    string curTune, curProfile;

    bool bFirstFrameProcessed;

    bool bUseCBR, bUseCFR, bPadCBR;

    list<VideoPacket> CurrentPackets;
    list<BYTE> HeaderPacket, SEIData;

    INT64 delayOffset;

    int frameShift;

    inline void ClearPackets()
    {
		list<VideoPacket> CurrentPackets;
		for (list<VideoPacket>::iterator itor = CurrentPackets.begin() ;
				itor != CurrentPackets.end(); ++itor)
		{
			(*itor).FreeData();
		}

        CurrentPackets.clear();
    }

    inline void SetBitRateParams(DWORD maxBitrate, DWORD bufferSize)
    {
        //-1 means ignore so we don't have to know both settings

        if (maxBitrate != -1)
            paramData.rc.i_vbv_max_bitrate  = maxBitrate; //vbv-maxrate

        if (bufferSize != -1)
            paramData.rc.i_vbv_buffer_size  = bufferSize; //vbv-bufsize

        if(bUseCBR)
            paramData.rc.i_bitrate = maxBitrate;
    }

public:
    X264Encoder(int fps, int width, int height, int quality/*建议设置成1*/,  int maxBitrate, int bufferSize, bool bUseCFR)
    {
        fps_ms = 1000/fps;

		curProfile = "main"; // AppConfig->GetString(TEXT("Video Encoding"), TEXT("X264Profile"), TEXT("high"));

        ZeroMemory(&paramData, sizeof(paramData));


		if (x264_param_default_preset(&paramData, "veryfast", NULL))
		{
			DOLOG("Failed to set default preset");
		}

        this->width  = width;
        this->height = height;

        paramData.b_deterministic = false;

		bUseCBR = false;
		bPadCBR = false;
        this->bUseCFR = false;

        SetBitRateParams(maxBitrate, bufferSize);

		//-------------- 和是否是CBR有关
        paramData.rc.i_rc_method    = X264_RC_CRF;
        paramData.rc.f_rf_constant  = baseCRF + float(10-quality);	// 暂时采用固定公式。还不清楚到底怎么影响

        UINT keyframeInterval = 0;									// 关键帧间隔

        paramData.b_vfr_input           = true;
        paramData.i_width               = width;
        paramData.i_height              = height;

		// 暂时不知道是做什么的，就采用默认，并不设置
        //paramData.vui.b_fullrange       = colorDesc.fullRange;
        //paramData.vui.i_colorprim       = colorDesc.primaries;
        //paramData.vui.i_transfer        = colorDesc.transfer;
        //paramData.vui.i_colmatrix       = colorDesc.matrix;

        if (keyframeInterval)
            paramData.i_keyint_max      = fps*keyframeInterval;

        paramData.i_fps_num             = fps;
        paramData.i_fps_den             = 1;

        paramData.i_timebase_num        = 1;
        paramData.i_timebase_den        = 1000;

        paramData.pf_log                = get_x264_log;
        paramData.i_log_level           = X264_LOG_WARNING;

        paramData.i_level_idc = 41; // to ensure compatibility with portable devices

        paramData.i_csp = X264_CSP_I420;

		if (x264_param_apply_profile(&paramData, "main"))
		{
			DOLOG("Failed to set x264 profile: main");
		}

        x264 = x264_encoder_open(&paramData);
		if (!x264)
		{
			DOLOG("Could not initialize x264");
		}

        DataPacket packet;
        GetHeaders(packet);
    }

    ~X264Encoder()
    {
        ClearPackets();
        x264_encoder_close(x264);
    }

    bool Encode(LPVOID picInPtr, LPVOID nalOut, int *pNalNum)
    {
        x264_picture_t *picIn = (x264_picture_t*)picInPtr;
        x264_picture_t picOut;
		x264_picture_init(&picOut);

        if(x264_encoder_encode(x264, (x264_nal_t**)&nalOut, pNalNum, picIn, &picOut) < 0)
        {
            DOLOG("x264 encode failed");
            return false;
        }
		int countCacheFrame = x264_encoder_delayed_frames(x264);
		DOLOG("当前被缓存的帧数为" + countCacheFrame);
		int res = 0;
		
		if (countCacheFrame>0)
		{
			res = x264_encoder_encode(x264, (x264_nal_t**)&nalOut, pNalNum, NULL, &picOut);
		}

        return true;
    }

    void GetHeaders(DataPacket &packet)
    {
		/*
        if(!HeaderPacket.Num())
        {
            x264_nal_t *nalOut;
            int nalNum;

            x264_encoder_headers(x264, &nalOut, &nalNum);

            for(int i=0; i<nalNum; i++)
            {
                x264_nal_t &nal = nalOut[i];

                if(nal.i_type == NAL_SPS)
                {
                    BufferOutputSerializer headerOut(HeaderPacket);

                    headerOut.OutputByte(0x17);
                    headerOut.OutputByte(0);
                    headerOut.OutputByte(0);
                    headerOut.OutputByte(0);
                    headerOut.OutputByte(0);
                    headerOut.OutputByte(1);
                    headerOut.Serialize(nal.p_payload+5, 3);
                    headerOut.OutputByte(0xff);
                    headerOut.OutputByte(0xe1);
                    headerOut.OutputWord(htons(nal.i_payload-4));
                    headerOut.Serialize(nal.p_payload+4, nal.i_payload-4);

                    x264_nal_t &pps = nalOut[i+1]; //the PPS always comes after the SPS

                    headerOut.OutputByte(1);
                    headerOut.OutputWord(htons(pps.i_payload-4));
                    headerOut.Serialize(pps.p_payload+4, pps.i_payload-4);
                }
            }
        }

        packet.lpPacket = HeaderPacket.Array();
        packet.size     = HeaderPacket.Num();
		*/
    }

    int GetBitRate() const
    {
        if (paramData.rc.i_vbv_max_bitrate)
            return paramData.rc.i_vbv_max_bitrate;
        else
            return paramData.rc.i_bitrate;
    }

	string GetInfoString() const
    {
        string strInfo;

		strInfo += "Video Encoding: x264";
		strInfo += "\r\n    fps: ";
		strInfo += paramData.i_fps_num;
		strInfo += "\r\n    width: ";
		strInfo += width;
		strInfo += "\r\n    profile: ";
		strInfo += curProfile;
		strInfo += "\r\n    keyint : ";
		strInfo += paramData.i_keyint_max;
		strInfo += "\r\n    CBR: ";
		strInfo += bUseCBR;
		strInfo += "\r\n    CFR: ";
		strInfo += bUseCFR;
		strInfo += "\r\n    max bitrate: ";
		strInfo += paramData.rc.i_vbv_max_bitrate;
		strInfo += "\r\n    buffer size: ";
		strInfo += paramData.rc.i_vbv_buffer_size;

        if(!bUseCBR)
        {
			strInfo += "\r\n    quality: ";
			strInfo += 10 - paramData.rc.f_rf_constant - baseCRF;
        }

        return strInfo;
    }

    virtual bool DynamicBitrateSupported() const
    {
        return (paramData.i_nal_hrd != X264_NAL_HRD_CBR);
    }

    virtual bool SetBitRate(DWORD maxBitrate, DWORD bufferSize)
    {
        DWORD old_bitrate = paramData.rc.i_vbv_max_bitrate;
        DWORD old_buffer  = paramData.rc.i_vbv_buffer_size;

        SetBitRateParams(maxBitrate, bufferSize);

        int retVal = x264_encoder_reconfig(x264, &paramData);
		if (retVal < 0)
		{
            DOLOG("Could not set new encoder bitrate, error value " + retVal);
		}
		else
		{
			if (old_bitrate != maxBitrate){
				DOLOG("bitrate " + old_bitrate + "->" + maxBitrate);
			}
			if (old_buffer != bufferSize){
				DOLOG("bitrate size " + old_buffer + "->" + bufferSize);
			}
        }

        return retVal == 0;
    }

    virtual void RequestKeyframe()
    {
        bRequestKeyframe = true;
    }

    virtual int GetBufferedFrames()
    {
        return x264_encoder_delayed_frames(x264);
    }

    virtual bool HasBufferedFrames()
    {
        return x264_encoder_delayed_frames(x264) > 0;
    }
};


VideoEncoder* CreateX264Encoder(int fps, int width, int height, int quality, int maxBitRate, int bufferSize, bool bUseCFR)
{
    return new X264Encoder(fps, width, height, quality, maxBitRate, bufferSize, bUseCFR);
}
