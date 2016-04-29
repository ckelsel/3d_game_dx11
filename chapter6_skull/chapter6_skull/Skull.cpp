/*
 * Copyright 2016 KunMing.Xie@hotmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class Skull : public D3DApp
{
public:

	Skull (HINSTANCE hInstance);
	~Skull();

	
	virtual bool Init();
	virtual void OnResize(); 
	virtual void UpdateScene(float dt);
	virtual void DrawScene(); 

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);

private:

	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer *m_VertexBuffer;
	ID3D11Buffer *m_IndexBuffer;

	ID3D11InputLayout *m_InputLayout;

	ID3DX11Effect* m_FX;
	ID3DX11EffectTechnique *m_Tech;
	ID3DX11EffectMatrixVariable *m_fxWorldViewProj;

	ID3D11RasterizerState *m_WireframeRS;

	XMFLOAT4X4 m_World;
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	UINT m_indexCount;
};

Skull::Skull (HINSTANCE hInstance)
	: D3DApp(hInstance)
	, mTheta(1.5f*MathHelper::Pi), mPhi(0.1f*MathHelper::Pi), mRadius(20.0f)
{
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_View, I);
	XMStoreFloat4x4(&m_Proj, I);

	XMMATRIX T = XMMatrixTranslation(0.0f, -2.0f, 0.0f);
	XMStoreFloat4x4(&m_World, T);
}

Skull::~Skull()
{
}

bool Skull::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(wireframeDesc));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &m_WireframeRS));

	return true;
}

void Skull::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}

void Skull::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, V);
}

void Skull::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0F, 0);

	md3dImmediateContext->IASetInputLayout(m_InputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	md3dImmediateContext->RSSetState(m_WireframeRS);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	
#if 0
	XMMATRIX view  = XMLoadFloat4x4(&m_View);
	XMMATRIX proj  = XMLoadFloat4x4(&m_Proj);
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX worldViewProj = world*view*proj;
#else
	// failed for what?
	XMMATRIX worldViewProj = XMLoadFloat4x4(&m_View) * XMLoadFloat4x4(&m_Proj) * XMLoadFloat4x4(&m_World);
#endif
	m_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));


	D3DX11_TECHNIQUE_DESC techDesc;
	m_Tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_Tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(m_indexCount, 0, 0);
	}

	HR(mSwapChain->Present(0, 0));
}

void Skull::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void Skull::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Skull::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.05f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.05f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 5.0f, 50.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}



void Skull::BuildGeometryBuffers()
{
	std::ifstream fin("Modules/skull.txt");
	
	if (!fin)
	{
		MessageBox(0, L"Modules/skull.txt not found", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	float nx, ny, nz;
	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	std::vector<Vertex> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		vertices[i].Color = black;

		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		
		// ignore
		fin >> nx >> ny >> nz;
	}

	/*
	}
	TriangleList
	{
	*/
	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	m_indexCount = 3 * tcount;
	std::vector<UINT> indices(m_indexCount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();
	

	//
	// Create Vertex Buffer
	//
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];

	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &m_VertexBuffer));

	//
	// Create Index Buffer
	//
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * m_indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];

	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer));
}

void Skull::BuildFX()
{
	std::ifstream fin("fx/color.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, md3dDevice, &m_FX));


	m_Tech = m_FX->GetTechniqueByName("ColorTech");
	m_fxWorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();

}

void Skull::BuildVertexLayout()
{
	//
	// Create Vertex Input Layout
	//
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	m_Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_InputLayout));
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	Skull theApp(hInstance);

	if (!theApp.Init())
		return 0;
	
	return theApp.Run();
}