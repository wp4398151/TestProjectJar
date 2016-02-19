#include "TextureClass.h"
#include <assert.h>

TextureClass::TextureClass()
{
}


TextureClass::~TextureClass()
{

}

TextureClass::TextureClass(const TextureClass& textureClass)
{
	m_pTextureView = NULL;
}

bool TextureClass::Initialize(ID3D11Device* pDevice, WCHAR *lpwFileName)
{
	HRESULT result = S_OK;
	result = D3DX11CreateShaderResourceViewFromFileW(pDevice, lpwFileName, NULL, NULL, &m_pTextureView, NULL);
	assert(SUCCEEDED(result));
	return SUCCEEDED(result);
}

bool TextureClass::Shutdown()
{
	if (m_pTextureView)
	{
		m_pTextureView->Release();
		m_pTextureView = NULL;
	}
	return true;
}