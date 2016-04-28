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

class BoxApp : public D3DApp
{
public:

	BoxApp (HINSTANCE hInstance);
	~BoxApp();

	
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

private:
	ID3D11Buffer *m_BoxVB;

	ID3DX11Effect* m_FX;
	ID3DX11EffectTechnique *m_Tech;
	ID3DX11EffectMatrixVariable *m_fxWorldViewProj;
};

void BoxApp::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob *compiledShader = 0;
	ID3D10Blob *compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 
		0, md3dDevice, &m_FX));

	ReleaseCOM(compiledShader);

	m_Tech = m_FX->GetTechniqueByName("ColorTech");
	m_fxWorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void BoxApp::BuildGeometryBuffers()
{
	Vertex vertices[] = 
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float *)&Colors::White	},
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float *)&Colors::Black	},
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;

	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &m_BoxVB));
}

BoxApp::BoxApp (HINSTANCE hInstance)
	: D3DApp (hInstance)
	, m_BoxVB (0)
{
}

BoxApp::~BoxApp()
{
}


bool BoxApp::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildGeometryBuffers();

	BuildFX();

	return true;
}

void BoxApp::OnResize()
{
}

void BoxApp::UpdateScene(float dt)
{
}

void BoxApp::DrawScene()
{
}


// Convenience overrides for handling mouse input.
void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	BoxApp theApp(hInstance);

	if (!theApp.Init())
		return 0;
	
	return theApp.Run();
}