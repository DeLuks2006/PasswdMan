#include "gui.h"
#include <windows.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

// window proc handler
long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter)) {
		return true;
	}

	switch (message) {
		// handle the size
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED) {
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(wideParameter);
			gui::ResetDevice();
		}
	}return 0;

		// i forgor
	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xffff0) == SC_KEYMENU) { // disable alt application menu
			return 0;
		}
	}break;

		// close the window
	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

		// handle the clicks
	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

		// move the window
	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON) {
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 && gui::position.x <= gui::WIDTH && gui::position.y >= 0 && gui::position.y <= 19) {
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0,
					0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
			}

		}
	}return 0;

	}
	return DefWindowProc(window, message, wideParameter, longParameter);
}

// handle creation of window
void gui::CreateHWindow(const char* windowName, const char* className) noexcept {
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		0,
		className,
		windowName,	// name of window
		WS_POPUP,	// popup
		100,	// start pos
		100,	// start pos
		WIDTH,	// width
		HEIGHT,	// height
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);

}

// handle destruction of window
void gui::DestroyHWindow() noexcept {
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

// device creation
bool gui::CreateDevice() noexcept {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d) {
		return false;
	}
	ZeroMemory(&presentParameters, sizeof(presentParameters));
	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &device) < 0) {
		return false;
	}
	return true;
}
// reset device
void gui::ResetDevice() noexcept {
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL) {
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();
}
// device destruction
void gui::DestroyDevice() noexcept {
	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}

// creation of imgui
void gui::CreateImGui() noexcept {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark(); // change this for own colors

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}
// destruction imgui
void gui::DestroyImGui() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// begin render
void gui::BeginRender() noexcept {
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	// start the frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
// end render
void gui::EndRender() noexcept {
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ResetDevice();
	}
}
// render
void gui::Render() noexcept {
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"PASSWDMAN!!!", 
		&exit, 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoSavedSettings | 
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoMove 
	);
	/*
	ImGui::Text("CLICK THA BUTTON!!");
	ImGui::SameLine();
	ImGui::Text("... Or don't");

	if (ImGui::Button("VERY COOL BUTTON") == true) {
		MessageBoxW(NULL, L"YOU CLICKED THA BUTTONE!", L"YOU DID THE THING", 0x00004000L);
	}

	static bool toggle = false;
	if (ImGui::Checkbox("checkbox...", &toggle)) {
		MessageBoxW(NULL, L"YOU CLICKED THA CHECKBOX", L"YOU DID THE THING", 0x00004000L);
	}
	*/
	ImGui::Columns(2);

	static bool bButton1Pressed = false;
	static bool bButton2Pressed = false;

	if (ImGui::BeginChild(1)) {
		if (ImGui::Button("< Dummy-Button 1 >", ImVec2(225, 25))) {
			bButton1Pressed = true;
			bButton2Pressed = false;
		}
		if (ImGui::Button("< Dummy-Button 2 >", ImVec2(225, 25))) {
			bButton2Pressed = true;
			bButton1Pressed = false;
		}
		ImGui::Button("< Dummy-Button 3 >", ImVec2(225, 25));
		ImGui::Button("< Dummy-Button 4 >", ImVec2(225, 25));
		ImGui::Button("< Dummy-Button 5 >", ImVec2(225, 25));

		ImGui::EndChild();
	}


	ImGui::NextColumn();
	if (bButton1Pressed == true) {
		ImGui::Text("< Dummy-Password 1 >");
		ImGui::NewLine();
		ImGui::Text("Username: < Dummy-Username 1 >");
		ImGui::NewLine();
		ImGui::Text("Password: < Dummy-Hidden_Password 1 >");

	}
	else if (bButton2Pressed == true) {
		ImGui::Text("< Dummy-Password 2 >");
		ImGui::NewLine();
		ImGui::Text("Username: < Dummy-Username 2 >");
		ImGui::NewLine();
		ImGui::Text("Password: < Dummy-Hidden_Password 2 >");
	}
	else {
		ImGui::Button("< Dummy-Button 1 >");
		ImGui::Button("< Dummy-Button 2 >");
	}
	

	ImGui::End();
}