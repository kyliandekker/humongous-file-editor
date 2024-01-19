#include <windows.h>
#include <thread>
#include <imgui/backends/imgui_impl_win32.h>

#include "imgui/ImGuiWindow.h"
#include "imgui/tools/ExplorerWindow.h"
#include "imgui/tools/ResourcesWindow.h"
#include "system/AudioSystem.h"
#include "low_level/FileContainer.h"

const char g_szClassName[] = "Humongous Resource Editor";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	resource_editor::imgui::window.ProcessEvents(hwnd, msg, wParam, lParam);
	switch (msg)
	{
		case WM_CLOSE:
		{
			resource_editor::imgui::window.Stop();
			resource_editor::imgui::window.DeleteWindow();
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_SIZE:
		{
			float width = LOWORD(lParam);
			float height = HIWORD(lParam);
			resource_editor::imgui::window.SetSize(ImVec2(width, height));
			break;
		}
		default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	return 0;
}

void UpdateRenderWindow()
{
	while (resource_editor::imgui::window.IsEnabled())
	{
		resource_editor::imgui::window.Render();
	}
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nShowCmd)
{
	WNDCLASSEX wc{};
	HWND hwnd;
	MSG Msg;

	AllocConsole();
	FILE* fConsole = nullptr;
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCWSTR)g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		(LPCWSTR)g_szClassName,
		L"Humongous Resource Editor",
		WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 1080, 720,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	resource_editor::imgui::window.SetHwnd(hwnd, wc);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	resource_editor::imgui::window.Initialize();

	std::thread renderThread = std::thread(&UpdateRenderWindow);
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	resource_editor::audioSystem.Stop();

	renderThread.join();

	DestroyWindow(hwnd);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

	fclose(fConsole);
	return static_cast<int>(Msg.wParam);
}