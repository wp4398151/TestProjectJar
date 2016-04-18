#include "CopyTexture2D.h"
#include <assert.h>
CopyTexture2D::CopyTexture2D()
{
	m_pCopyTexture = NULL;
}

CopyTexture2D::~CopyTexture2D()
{
	if (m_pCopyTexture )
	{
		m_pCopyTexture->Release();
		m_pCopyTexture = NULL;
	}
}

void CopyTexture2D::Copy(ID3D11DeviceContext* pDeviceContext,
	ID3D11Device* pDeivce,
	IDXGISwapChain* pSwapChain)
{
	ID3D11Texture2D *pRenderTexture = NULL;
	HRESULT hr = S_OK;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pRenderTexture);
	assert(SUCCEEDED(hr));
	D3D11_TEXTURE2D_DESC renderDesc;
	pRenderTexture->GetDesc(&renderDesc);
	// create texture for receive render texture
	
	D3D11_TEXTURE2D_DESC copyDesc;
	ZeroMemory(&copyDesc, sizeof(&copyDesc));
	copyDesc.Width = renderDesc.Width;
	copyDesc.Height = renderDesc.Height;
	copyDesc.MipLevels = renderDesc.MipLevels;
	copyDesc.ArraySize= renderDesc.ArraySize;
	copyDesc.Format = renderDesc.Format;
	copyDesc.SampleDesc.Count = renderDesc.SampleDesc.Count;
	copyDesc.SampleDesc.Quality = renderDesc.SampleDesc.Quality;
	copyDesc.Usage = D3D11_USAGE_STAGING;
	copyDesc.MiscFlags = 0;
	copyDesc.BindFlags = 0;
	copyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	
	hr = pDeivce->CreateTexture2D(&copyDesc, NULL, &m_pCopyTexture);
	assert(SUCCEEDED(hr));
	
	pDeviceContext->CopyResource((ID3D11Resource*)m_pCopyTexture,(ID3D11Resource*)pRenderTexture);

	if (pRenderTexture)
	{
		pRenderTexture->Release();
		pRenderTexture = NULL;
	}

	return ;
}

void CopyTexture2D::CheckCopyTextureContent(ID3D11DeviceContext* pDeviceContext)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));
	hr = pDeviceContext->Map((ID3D11Resource*)m_pCopyTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
	assert(SUCCEEDED(hr));
	int a = 1;
	int b = 2;
	int c = a + b;
	pDeviceContext->Unmap((ID3D11Resource*)m_pCopyTexture, 0);
}