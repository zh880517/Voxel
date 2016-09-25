#include "RenderTestMesh.h"
#include <Core/DXUT.h>
#include "RenderObjShader.h"
#include <Optional/DXUTcamera.h>
#include <MeshLoadObj.h>
using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

RenderTestMesh::RenderTestMesh()
	: m_pIndexBuffer(nullptr)
	, m_pVerterBuffer(nullptr)
	, m_pRenderShader(nullptr)
	, m_iIndexCount(0)
	, m_bLoadOK(false)
{
}

RenderTestMesh::~RenderTestMesh()
{
	Destory();
}

bool RenderTestMesh::Create(ID3D11Device * pd3dDevice)
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVerterBuffer);
	
	m_iIndexCount = 36;

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVerterBuffer);
	if (FAILED(hr))
		return false;

	uint32_t indices[] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(uint32_t) * 36;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	if (FAILED(hr))
		return false;

	m_bLoadOK = true;
	return true;
}

void RenderTestMesh::SetRenderObjShader(RenderObjShader * pShader)
{
	m_pRenderShader = pShader;
}

void RenderTestMesh::OnRender(ID3D11DeviceContext * pd3dImmediateContext, CModelViewerCamera * pCamera)
{
	if (!m_bLoadOK)
		return;
	static DirectX::XMMATRIX s_defult = DirectX::XMMatrixIdentity();
	if (m_pRenderShader)
	{
		m_pRenderShader->OnRender(pd3dImmediateContext, pCamera->GetProjMatrix(), pCamera->GetViewMatrix(), s_defult);
	}
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVerterBuffer, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dImmediateContext->DrawIndexed(36, 0, 0);
}

void RenderTestMesh::Destory()
{
	m_bLoadOK = false;
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVerterBuffer);
}
