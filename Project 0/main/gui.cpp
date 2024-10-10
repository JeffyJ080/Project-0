#include "gui.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

// imported windows process manager
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND Window, 
	UINT Message,
	WPARAM wideParameter, 
	LPARAM longParameter
);

// our own windows process manager
long __stdcall WindowProcess(
	HWND Window,
	UINT Message,
	WPARAM wideParameter,
	LPARAM longParameter
)
{
	if (ImGui_ImplWin32_WndProcHandler(Window, Message, wideParameter, longParameter))
		return true;

	switch (Message)
	{
		// resize algorithm
		case WM_SIZE:
		{
			if (gui::device && wideParameter != SIZE_MINIMIZED)
			{
				gui::presentPerameters.BackBufferWidth = LOWORD(longParameter);
				gui::presentPerameters.BackBufferHeight = HIWORD(longParameter);

				gui::ResetDevice();
			}
		}return 0;

		case WM_SYSCOMMAND:
		{
			if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
		}break;

		case WM_DESTROY:
		{
			gui::exit = false;
			PostQuitMessage(0);
		}return 0;

		case WM_LBUTTONDOWN:
		{
			gui::position = MAKEPOINTS(longParameter); // set click points
		}return 0;

		case WM_MOUSEMOVE:
		{
			if (wideParameter == MK_LBUTTON)
			{
				const auto points = MAKEPOINTS(longParameter);
				auto rect = ::RECT{ };

				GetWindowRect(gui::window, &rect);

				rect.left += points.x - gui::position.x;
				rect.top += points.y - gui::position.y;

				if (gui::position.x >= 0 &&
					gui::position.x <= gui::window_width &&
					gui::position.y >= 0 && gui::position.y <= 15)
					SetWindowPos(
						gui::window,
						HWND_TOPMOST,
						rect.left,
						rect.top,
						0, 0,
						SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
					);
			}
		}return 0;

	}

	return DefWindowProc(Window, Message, wideParameter, longParameter);
}

void gui::CreateHWindow(
	const char* windowName,
	const char* className) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA(
		className,
		windowName,
		WS_POPUP | WS_SIZEBOX,
		100, // area where window will apear
		100, // area where window will apear
		window_width,
		window_height,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow()noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentPerameters, sizeof(presentPerameters));

	presentPerameters.Windowed = TRUE;
	presentPerameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentPerameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentPerameters.EnableAutoDepthStencil = TRUE;
	presentPerameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentPerameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentPerameters,
		&device) < 0)
		return false;

	return true;
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentPerameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	//io.IniFilename = NULL;

	ImGui::StyleColorsDark(0); // Default style

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	// Start Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device ->SetRenderState(D3DRS_ZENABLE, FALSE);
	device ->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

using namespace ImGui;

void gui::Render() noexcept
{
	// Variables

	// Dock Window
	DockSpaceOverViewport();
	
	// Demo Window
	ShowDemoWindow();

	 //This is where we state what needs to be rendered
	//ImGui::SetNextWindowPos({ 0, 0 });
	//ImGui::SetNextWindowSize({ window_width, window_height });

	Begin("Project 0", &exit, ImGuiWindowFlags_NoResize);

	//// Rendering code goes here
	if (Button("Hello, world!"))
	{
		MessageBox(0, "Hello, world!", "Hello, world!", 0);
	}
	SetItemTooltip("This is an example tooltip");

	Text("Hello, world!");
	SetItemTooltip("This is an example tooltip 1");

	End();

	// ImGui Window
	Begin("Project 1");

	Button("Hello, world!");
	SetItemTooltip("This is an example tooltip");

	End();

	// Menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}

		SetCursorPosX(GetWindowWidth() - 50);
		if (ImGui::BeginMenu("_"))
		{

		}
		if (ImGui::BeginMenu("X"))
		{

		}
		ImGui::EndMainMenuBar();
	}
}