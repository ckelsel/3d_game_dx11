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


class BoxApp : public D3DApp
{
public:

	BoxApp (HINSTANCE hInstance);
	~BoxApp();

	
	virtual bool Init();
	virtual void OnResize(); 
	virtual void UpdateScene(float dt);
	virtual void DrawScene(); 
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
};

BoxApp::BoxApp (HINSTANCE hInstance)
	: D3DApp (hInstance)
{
}

BoxApp::~BoxApp()
{
}


bool BoxApp::Init()
{
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

LRESULT BoxApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
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