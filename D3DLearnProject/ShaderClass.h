#pragma once

#include <windows.h>
#include <D3D11.h>
#include <D3DX10math.h>

class ShaderClass
{
public:
	bool Init(ID3D11Device *pDevice, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext *pDeviceContext, 
					int indexCount,
					D3DXMATRIX worldMatrix,
					D3DXMATRIX viewMatrix,
					D3DXMATRIX projectionMatrix,
					ID3D11ShaderResourceView* pShaderResourceView);
private:
	bool RenderShader(ID3D11DeviceContext *pDeviceContext,
						int indexCount);
	bool SetShaderParam(ID3D11DeviceContext *pDeviceContext,
						D3DXMATRIX worldMatrix,
						D3DXMATRIX viewMatrix,
						D3DXMATRIX projectionMatrix,
						ID3D11ShaderResourceView* pShaderResourceView);

private:
	ID3D11InputLayout *m_pLayout;
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pPixelShader;
	ID3D11Buffer *m_pConstantBuffer;
	//ID3DBlob *m_pMatrixBuffer
	ID3D11SamplerState *m_pSampleState;
};