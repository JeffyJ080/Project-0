#pragma once

#include <d3d9.h>

namespace gui
{
	// constant window size
	constexpr int window_width = 800;
	constexpr int window_height = 600;

	inline bool exit = true;

	// winapi window vars
	inline HWND window = nullptr;
	inline WNDCLASSEXA window_class = {  };

	// points for window movement
	inline POINTS position = { };

	// direct x state vars
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentPerameters = { };

	// window creation and destruction
	void CreateHWindow(
		const char* windowName,
		const char* className) noexcept;
	void DestroyHWindow()noexcept;

	// handle device creation and destruction
	bool CreateDevice() noexcept;
	void DestroyDevice() noexcept;
	void ResetDevice() noexcept;

	// creation and destruction of ImGui
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
}