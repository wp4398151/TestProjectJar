#include "TextureManager.h"
#include <assert.h>
#include <D3DX11tex.h>
#include "Helper.h"

TextureManager::TextureManager()
{
	m_pSamplerState = NULL;
}

TextureManager::~TextureManager()
{
	m_pSamplerState = NULL;
}

bool TextureManager::Initialize(ID3D11Device* pDevice)
{
	assert(pDevice);
	// 创建纹理采样描述
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// 线性插值方式采样
	// 地址模式
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// 创建纹理采样状态.
	HRESULT res = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	assert(SUCCEEDED(res));
	return true;
}

void TextureManager::Uninitialize()
{
	SAFE_RELEASE(m_pSamplerState);

	for (map<wstring, ID3D11ShaderResourceView*>::iterator itor = m_TextureMap.begin();
		itor != m_TextureMap.end(); )
	{
		SAFE_RELEASE(itor->second);
		m_TextureMap.erase(itor++);
	}
}

// 支持jpg
ID3D11ShaderResourceView* 
TextureManager::LoadTextureFromFileW(
		ID3D11Device* pDevice, LPWSTR pWTextureFilePath)
{
	assert(pWTextureFilePath);
	wstring jpgPath;
	GetAppPathW(jpgPath);
	jpgPath += L"\\";
	jpgPath += pWTextureFilePath;

	ID3D11ShaderResourceView* pShaderResourceView = NULL;
	assert(pDevice);
	map<wstring, ID3D11ShaderResourceView*>::iterator itor = m_TextureMap.find(pWTextureFilePath);
	if (itor != m_TextureMap.end())
	{
		pShaderResourceView = itor->second;
	}
	else
	{
		HRESULT res = S_OK;
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
		loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_BC3_UNORM;
		loadInfo.MipLevels = D3DX11_DEFAULT; //这时会产生最大的mipmaps层。 
		loadInfo.MipFilter = D3DX11_FILTER_LINEAR;

		res = D3DX11CreateShaderResourceViewFromFileW(pDevice, jpgPath.c_str(), 
				&loadInfo, NULL, &pShaderResourceView, NULL);
		assert(SUCCEEDED(res));

		//------------------------------
		// 查看加载的纹理的格式	
		ID3D11Texture2D *pTex2D = NULL;
		pShaderResourceView->GetResource((ID3D11Resource**)&pTex2D);
		assert(pTex2D);
		D3D11_TEXTURE2D_DESC tex2Ddesc;
		pTex2D->GetDesc(&tex2Ddesc);
		//------------------------------

		m_TextureMap.insert(make_pair(pWTextureFilePath, pShaderResourceView));
		pShaderResourceView = pShaderResourceView;
	}
	return pShaderResourceView;
}

ID3D11SamplerState *TextureManager::GetSamplaState()
{
	assert(m_pSamplerState);
	return m_pSamplerState;
}