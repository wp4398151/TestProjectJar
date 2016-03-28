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

ID3D11ShaderResourceView* 
TextureManager::LoadTextureFromFile(
		ID3D11Device* pDevice, wstring &rWTextureFilePath)
{
	ID3D11ShaderResourceView* pShaderResourceView = NULL;
	assert(pDevice);
	map<wstring, ID3D11ShaderResourceView*>::iterator itor = m_TextureMap.find(rWTextureFilePath);
	if (itor != m_TextureMap.end())
	{
		pShaderResourceView = itor->second;
	}
	else
	{
		HRESULT res = S_OK;
		res = D3DX11CreateShaderResourceViewFromFileW(pDevice, rWTextureFilePath.c_str(), 
				NULL, NULL, &pShaderResourceView, NULL);
		assert(SUCCEEDED(res));

		m_TextureMap.insert(make_pair(rWTextureFilePath, pShaderResourceView));
		pShaderResourceView = pShaderResourceView;
	}
	return pShaderResourceView;
}

ID3D11SamplerState *TextureManager::GetSamplaState()
{
	assert(m_pSamplerState);
	return m_pSamplerState;
}