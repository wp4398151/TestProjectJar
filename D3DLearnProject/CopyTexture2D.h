#pragma once
#include <d3d11.h>

class CopyTexture2D
{
public:
	CopyTexture2D();
	~CopyTexture2D();
	void Copy(ID3D11DeviceContext* pDeviceContext,
				ID3D11Device* pDeivce,
					IDXGISwapChain* swapChain);
	void CheckCopyTextureContent(ID3D11DeviceContext* pDeviceContext);
private:
	ID3D11Texture2D *m_pCopyTexture;
};

