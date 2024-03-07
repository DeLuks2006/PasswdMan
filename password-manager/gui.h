#pragma once
#include <d3d9.h>

namespace gui {

	constexpr int WIDTH = 550;	// window height
	constexpr int HEIGHT = 300; // window width

	inline bool exit = true;	// while exit is true, we are not exiting the window :))

	// winAPI variables
	inline HWND window = nullptr;
	inline WNDCLASSEXA windowClass = { };

	// handle window movement
	inline POINTS position = { };

	// directX state vars
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = { };

	/*---------[ FUNCTION-PROTOTYPES ]---------*/

	// handle creation of window
	void CreateHWindow(
		const char* windowName,
		const char* className) noexcept;

	// handle destruction of window
	void DestroyHWindow() noexcept;

	// device creation
	bool CreateDevice() noexcept;
	// reset device
	void ResetDevice() noexcept;
	// device destruction
	void DestroyDevice() noexcept;

	// creation of imgui
	void CreateImGui() noexcept;
	// destruction imgui
	void DestroyImGui() noexcept;

	// begin render
	void BeginRender() noexcept;
	// end render
	void EndRender() noexcept;
	// render
	void Render() noexcept;
}
