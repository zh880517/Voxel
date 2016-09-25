#include "RenderFrameShader.h"
#include <Core/DXUT.h>

using namespace DirectX;
struct CBChangesEveryFrame
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

RenderFrameShader::RenderFrameShader()
	: m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pVertexLayout(nullptr)
	, m_pCBChangesEveryFrame(nullptr)
	, m_pRasterizerState(nullptr)
{
}

RenderFrameShader::~RenderFrameShader()
{
	Destory();
}

bool RenderFrameShader::Create(ID3D11Device * pd3dDevice, LPCWSTR strFile)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileFromFile(strFile, nullptr, "VS", "vs_4_0", dwShaderFlags, 0, &pVSBlob);
	if (FAILED(hr))
		return false;

	hr = pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSBlob);
		return false;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pVertexLayout);
	SAFE_RELEASE(pVSBlob);
	if (FAILED(hr))
		return false;


	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileFromFile(strFile, nullptr, "PS", "ps_4_0", dwShaderFlags, 0, &pPSBlob);
	if (FAILED(hr))
		return false;
	// Create the pixel shader
	hr = pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	SAFE_RELEASE(pPSBlob);
	if (FAILED(hr))
		return false;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(CBChangesEveryFrame);
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pCBChangesEveryFrame);
	if (FAILED(hr))
		return false;

	D3D11_RASTERIZER_DESC rds;
	ZeroMemory(&rds, sizeof(rds));
	rds.CullMode = D3D11_CULL_BACK;
	rds.FillMode = D3D11_FILL_WIREFRAME;
	pd3dDevice->CreateRasterizerState(&rds, &m_pRasterizerState);

	return true;
}

void RenderFrameShader::OnRender(ID3D11DeviceContext * pd3dImmediateContext, const DirectX::XMMATRIX & mProj, const DirectX::XMMATRIX & mView, const DirectX::XMMATRIX& mWorld)
{
// 	HRESULT hr;
// 	D3D11_MAPPED_SUBRESOURCE MappedResource;
// 	V(pd3dImmediateContext->Map(m_pCBChangesEveryFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
// 	auto pCB = reinterpret_cast<CBChangesEveryFrame*>(MappedResource.pData);
// 	
// 	pCB->mWorld = DirectX::XMMatrixTranspose(mWorld);
// 	pCB->mView = DirectX::XMMatrixTranspose(mView);
// 	pCB->mProjection = DirectX::XMMatrixTranspose(mProj);
// 
// 	pd3dImmediateContext->Unmap(m_pCBChangesEveryFrame, 0);

	CBChangesEveryFrame cb;
	cb.mWorld = XMMatrixTranspose(mWorld);
	cb.mView = XMMatrixTranspose(mView);
	cb.mProjection = XMMatrixTranspose(mProj);
	pd3dImmediateContext->UpdateSubresource(m_pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0);

	pd3dImmediateContext->IASetInputLayout(m_pVertexLayout);
	pd3dImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &m_pCBChangesEveryFrame);
	pd3dImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
	//pd3dImmediateContext->PSSetConstantBuffers(0, 1, &m_pCBChangesEveryFrame);
	pd3dImmediateContext->RSSetState(m_pRasterizerState);
}

void RenderFrameShader::Destory()
{
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pCBChangesEveryFrame);
	SAFE_RELEASE(m_pRasterizerState);
}

HRESULT RenderFrameShader::CompileFromFile(LPCWSTR pFileName, const D3D_SHADER_MACRO * pDefines, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob ** ppCode)
{
	HRESULT hr;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	Flags1 |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(pFileName, pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pEntrypoint, pTarget, Flags1, Flags2,
		ppCode, &pErrorBlob);

	if (pErrorBlob)
	{
		OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
		pErrorBlob->Release();
	}

	return hr;
}
