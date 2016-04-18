#pragma once

#include <d3d11.h>
#include <D3DX10math.h>

class RectClass
{
public:
	RectClass();
	~RectClass();

	struct VertexType{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
		D3DXVECTOR3 normal;
	};

	bool Initialize(ID3D11Device* pDevice);
	void ShutDown();
	void Render(ID3D11DeviceContext* pDeviceContext,
			D3DXMATRIX worldMatrix,
				D3DXMATRIX viewMatrix,
					D3DXMATRIX projectionMatrix);
private:
	
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_vertexCount;
	int m_IndexCount;

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pLayout;
	ID3D11Buffer* m_pContantBuffer;
};

