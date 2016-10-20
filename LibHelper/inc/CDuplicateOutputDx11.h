/**
 * @file
 * @brief it's duplicate the screen, and retain the image.
 *
 * @author Wupeng <mailto:4398151@qq.com>
 *
 */
#pragma once
#include <Windows.h>
#include <D3D11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include "Helper.h"

enum DuplicatorInfo
{
	DuplicatorInfo_Error,		// need to stop recorder
	DuplicatorInfo_Timeout,		// need re acquire
	DuplicatorInfo_Lost,		// need recreate the duplication
	DuplicatorInfo_Acquired,	// succeeded
	DuplicatorInfo_InvalidCall
};

enum DupicatorStatus
{
	DupicatorStatus_Restart,
	DupicatorStatus_Exit,
};

/**
 * Main class for management the source of duplication
 */
class CDuplicateOutputDx11 : public LiteThread<CDuplicateOutputDx11>
{
public:
	CDuplicateOutputDx11(HWND hwnd) :m_pSwapChain(nullptr), m_pDevice(nullptr), 
		m_pOutputDuplication(nullptr), m_pDeviceContext(nullptr), LiteThread(this), m_hwnd(hwnd) {
	}
	bool Init();
	void Release();
	bool CreateOutputDuplicator();
	void DoneWithFrame();
	DuplicatorInfo GetNewFrame(ID3D11Texture2D** p2dTexture);
	static unsigned __stdcall RunFunc(void* pInst);
	DupicatorStatus GetStatus(){ return m_DuplicatorStatus; }

	void SaveTextureInBMP(ID3D11Texture2D* p2dTexture);
	bool CopyResource(ID3D11Texture2D **pTargetRenderTexture, ID3D11Texture2D *pSrcRenderTexture);
	bool GetSpecificAdapter(int idAdapter, IDXGIAdapter** pAdapter);
	DupicatorStatus m_DuplicatorStatus;
protected:
	HWND m_hwnd;
	IDXGISwapChain* m_pSwapChain;
	ID3D11DeviceContext* m_pDeviceContext;
	ID3D11Device* m_pDevice;
	IDXGIOutputDuplication* m_pOutputDuplication;
};
