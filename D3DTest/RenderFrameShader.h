#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "RenderObjShader.h"
class RenderFrameShader : public RenderObjShader
{
public:
	RenderFrameShader();
	~RenderFrameShader();

	bool	Create(ID3D11Device* pd3dDevice, LPCWSTR strFile);

	void	OnRender(ID3D11DeviceContext* pd3dImmediateContext, const DirectX::XMMATRIX& mProj, const DirectX::XMMATRIX& mView, const DirectX::XMMATRIX& mWorld);

	void	Destory();

protected:
	HRESULT CompileFromFile(LPCWSTR pFileName, const D3D_SHADER_MACRO* pDefines, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode);

private:
	ID3D11VertexShader*         m_pVertexShader;
	ID3D11PixelShader*          m_pPixelShader;
	ID3D11InputLayout*          m_pVertexLayout;
	ID3D11Buffer*               m_pCBChangesEveryFrame;
	ID3D11RasterizerState*		m_pRasterizerState;
};