/**
* @file
* @brief it's duplicate the screen, and retain the image.
*
* @author Wupeng <mailto:4398151@qq.com>
*
*/
#include "CDuplicateOutputDx11.h"
#include "TipMsgWindow.h"
#include "gtest/gtest.h"

TEST(CDuplicateOutputDx11, Initialize)
{
	TipMsgWindow tip;
	POINT pos = { 1, 1 };
	tip.Init("test Output", pos, pos);
	tip.ShowATopMostInfoWindow();
	HWND hwnd = tip.GetWnd();

	CDuplicateOutputDx11 output(hwnd);
	do
	{
		DOLOG("Start****************");
		output.close();
		output.Release();
		output.Init();
		output.start();
		WaitForSingleObject(output.m_hThread, INFINITE);
	} while (output.GetStatus() == DupicatorStatus_Restart);
	DOLOG("Exit******************");

}

void CDuplicateOutputDx11::Release()
{
	SAFE_RELEASE(m_pOutputDuplication);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pDevice);
}

void CDuplicateOutputDx11::DoneWithFrame()
{
	m_pOutputDuplication->ReleaseFrame();
}

DuplicatorInfo CDuplicateOutputDx11::GetNewFrame(ID3D11Texture2D** p2dTexture)
{
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	IDXGIResource* pDesktopResource = nullptr;
	HRESULT hr = S_OK;
	hr = m_pOutputDuplication->AcquireNextFrame(15, &frameInfo, &pDesktopResource);
	if (hr == DXGI_ERROR_ACCESS_LOST)
	{
		return DuplicatorInfo_Lost;
	}
	else if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	{
		return DuplicatorInfo_Timeout;
	}
	else if (hr == DXGI_ERROR_INVALID_CALL)
	{
		return DuplicatorInfo_InvalidCall;
	}
	else if (FAILED(hr))
	{
		return DuplicatorInfo_Error;
	}

	if (FAILED(hr = pDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(p2dTexture))))
	{
		SAFE_RELEASE(pDesktopResource);
		DOLOG(" pDesktopResource->QueryInterface failed");
		return DuplicatorInfo_Error;
	}
	
	SAFE_RELEASE(pDesktopResource);
	return DuplicatorInfo_Acquired;
}

unsigned __stdcall CDuplicateOutputDx11::RunFunc(void* pInst)
{
	assert(pInst);
	CDuplicateOutputDx11* pThis = reinterpret_cast<CDuplicateOutputDx11*>(pInst);
	if (!pThis->CreateOutputDuplicator())
	{
		pThis->m_DuplicatorStatus = DupicatorStatus_Restart;
		return 0;
	}

	DWORD startCount = 0;
	DWORD curTime = 0;
	startCount = GetTickCount();
	while (true)
	{
		ID3D11Texture2D* p2dTexture = nullptr;

		pThis->DoneWithFrame();
		DuplicatorInfo retInfo =  pThis->GetNewFrame(&p2dTexture);

		D3D11_TEXTURE2D_DESC desc2D;
		switch (retInfo)
		{
		case DuplicatorInfo_Acquired:
		{
			p2dTexture->GetDesc(&desc2D);
			DOLOG("GetNewFrame Lapse Acquired : " + (GetTickCount() - startCount) + "mili sec, x:" + desc2D.Width + ", y:" + desc2D.Height);
			startCount = GetTickCount();
			SAFE_RELEASE(p2dTexture);
			continue;
		}
		case DuplicatorInfo_Lost:
		{
			DOLOG("GetNewFrame Lapse Lost: " + (GetTickCount()- startCount) + "mili sec------------------------------------------------------------ \r\n");
			startCount = GetTickCount();
			if (false == pThis->CreateOutputDuplicator())
			{
				pThis->m_DuplicatorStatus = DupicatorStatus_Restart;
				DOLOG("GetNewFrame Lapse Acquired End Restart");
				return 0;
			}
			DOLOG("GetNewFrame Lapse Acquired Continue");
			continue;
		}
		case DuplicatorInfo_InvalidCall:
		{
			DOLOG("GetNewFrame Lapse invalid call: " + (GetTickCount()- startCount) + "mili sec<<<<<<<<< \r\n");
			startCount = GetTickCount();
			pThis->m_DuplicatorStatus = DupicatorStatus_Restart;
			return 0;
		}
		case DuplicatorInfo_Timeout:
		{
		   //pThis->DoneWithFrame();
			//DOLOG("GetNewFrame Lapse Timeout: " + (GetTickCount() - startCount) + "mili sec \r\n");
			continue;	
		}
		case DuplicatorInfo_Error:
		default:
		{
			startCount = GetTickCount();
			DOLOG("GetNewFrame Lapse Error: " + (GetTickCount() - startCount) + "mili sec \r\n");
			pThis->m_DuplicatorStatus = DupicatorStatus_Exit;
			return 0;
		}
		};
	}
	return 0;
}

bool CDuplicateOutputDx11::CreateOutputDuplicator()
{
	SAFE_RELEASE(m_pOutputDuplication);

	HRESULT hRes = S_OK;
	IDXGIDevice* pDxgiDevice = nullptr;

	hRes = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
	if (!SUCCEEDED(hRes))
	{
		DOLOG("m_pDevice->QueryInterface failed!");
		return false;
	}
	
	IDXGIAdapter* pDxgiAdapter = nullptr;
	hRes = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter));
	SAFE_RELEASE(pDxgiDevice);
	if (!SUCCEEDED(hRes))
	{
		DOLOG("pDxgiDevice->GetParent failed!");
		return false;
	}

	// Get output
	IDXGIOutput* pDxgiOutput = nullptr;
	hRes = pDxgiAdapter->EnumOutputs(0, &pDxgiOutput);
	SAFE_RELEASE(pDxgiAdapter);
	if (!SUCCEEDED(hRes))
	{
		DOLOG("pDxgiAdapter->EnumOutputs failed!");
		return false;
	}

	// Get output1
    IDXGIOutput1* pDxgiOutput1 = nullptr;
    hRes = pDxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pDxgiOutput1));
	SAFE_RELEASE(pDxgiOutput);
    if (!SUCCEEDED(hRes))
    {
		DOLOG("pDxgiOutput->QueryInterface failed!");
		return false;
    }
	
	// Get duplicate
	hRes = pDxgiOutput1->DuplicateOutput(m_pDevice, &m_pOutputDuplication);
	SAFE_RELEASE(pDxgiOutput1);
	if (!SUCCEEDED(hRes))
	{
		DOLOG("pDxgiOutput1->DuplicateOutput");
		return false;
	}
		
	return true;
}

bool CDuplicateOutputDx11::Init()
{
	HRESULT err = NULL;

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.Width = 1920;
	swapDesc.BufferDesc.Height = 1080;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hwnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.Windowed = TRUE;

	D3D_FEATURE_LEVEL desiredLevels[6] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	D3D_FEATURE_LEVEL receivedLevel;

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
	err = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, D3D11_CREATE_DEVICE_DEBUG, desiredLevels, 6, D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, &receivedLevel, &m_pDeviceContext);
	if (SUCCEEDED(err))
	{
		//m_pSwapChain->AddRef();
		//m_pDevice->AddRef(); 
		//m_pDeviceContext->AddRef();
		return true;
	}
	return false;
}

