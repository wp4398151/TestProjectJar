#pragma once

#include <d3d11.h>
#include <D3DX10math.h>

class LightBox
{
public:
	public:
		LightBox(void);
		LightBox(const LightBox& rBox);

		~LightBox(void);
		bool Initialize(ID3D11Device* pDevice);
		void Shutdown();
		void Render(ID3D11DeviceContext* pDeviceContext);

		int GetIndexCount();
		void GetWorldMatrix(D3DXMATRIX& m_WorldMatrix);

	private:
		bool InitializeBuffers(ID3D11Device* pDevice);
		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext*);

		//¶¥µã»º³åºÍ¶¥µãË÷Òý»º³å
		ID3D11Buffer *m_pVertexBuffer, *m_pIndexBuffer;
		D3DXMATRIX m_WorldMatrix;
		int m_vertexCount, m_indexCount;
};