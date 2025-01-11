#include <cmath>
#include <stdint.h>
#include <Windows.h>

#include <utils/logger.hpp>
#include "window.hpp"
#include "windowWin32.hpp"

bool g_IsInitialized = false;
FloppyBird::Windowing::Window* window = nullptr;
WPARAM sideBeingDragged = 0;
double aspectRatio = 640.0 / 480.0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (g_IsInitialized) {
		switch (message) {
		case WM_PAINT:
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		case WM_SIZE: {
			RECT clientRect = {};
			::GetClientRect(hwnd, &clientRect);

			int width = clientRect.right - clientRect.left;
			int height = clientRect.bottom - clientRect.top;

			if (window) {
				window->InvokeOnResize(width, height);
			}
		}
		break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			switch (wParam) {
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;

			case VK_F4:
				if (alt) {
					::PostQuitMessage(0);
				}
				break;

			default:
				window->InvokeOnKeypress(wParam);
			}
		}
		break;


		case WM_SIZING:
			sideBeingDragged = wParam;
			break;

		case WM_WINDOWPOSCHANGING:
		{
			WINDOWPOS* winPos = (WINDOWPOS*)lParam;

			// Adjust window dimensions to maintain aspect ratio
			switch (sideBeingDragged)
			{
			case WMSZ_BOTTOM:
			case WMSZ_TOPRIGHT:
				winPos->cx = (int)((double)winPos->cy * aspectRatio);
				break;

			case WMSZ_RIGHT:
			case WMSZ_BOTTOMLEFT:
			case WMSZ_BOTTOMRIGHT:
				winPos->cy = (int)((double)winPos->cx / aspectRatio);
				break;

			case WMSZ_TOP:
			{
				// Adjust the x position of the window to make it appear
				// that the bottom right side is anchored
				WINDOWPOS old = *winPos;

				winPos->cx = (int)((double)winPos->cy * aspectRatio);

				winPos->x += old.cx - winPos->cx;;
			}
			break;

			case WMSZ_LEFT:
			case WMSZ_TOPLEFT:
			{
				// Adjust the y position of the window to make it appear
				// the bottom right side is anchored. TOPLEFT resizing
				// will move the window around if you don't do this
				WINDOWPOS old = *winPos;
				winPos->cy = (int)((double)winPos->cx / aspectRatio);

				winPos->y += old.cy - winPos->cy;
			}
			break;
			}
		}
		break;

		// The default window procedure will play a system notification sound 
		// when pressing the Alt+Enter keyboard combination if this message is 
		// not handled.
		case WM_SYSCHAR:
			break;
		default:
			return ::DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
	else {
		return ::DefWindowProcW(hwnd, message, wParam, lParam);
	}

	return 0;
}

void RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName) {
	// Register a window class for creating our render window with.
	WNDCLASSEXW windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInst;
	windowClass.hIcon = ::LoadIcon(hInst, NULL);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = ::LoadIcon(hInst, NULL);

	static ATOM atom = ::RegisterClassExW(&windowClass);
	if (atom == 0) {
		F_LOG_FATAL(FloppyBird::Logger::LogSource::Windows, "Failed to register window class.");
	}
}

HWND SetupWindow(
	const wchar_t* windowClassName,
	HINSTANCE hInst,
	const wchar_t* windowTitle,
	uint32_t width, uint32_t height
) {
	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
	int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
	int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

	HWND hWnd = ::CreateWindowExW(
		NULL,
		windowClassName,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		hInst,
		nullptr
	);

	if (hWnd == NULL) {
		F_LOG_FATAL(FloppyBird::Logger::LogSource::Windows, "Failed to create a window.");
	}

	g_IsInitialized = true;

	return hWnd;
}

FloppyBird::Windowing::WindowWin32::WindowWin32(HINSTANCE hInstance) : hinstance(hInstance), hwnd(nullptr) {
	auto windowClassName = L"FloppyBirdWindowClass";

	RegisterWindowClass(hInstance, windowClassName);
	hwnd = SetupWindow(windowClassName, hInstance, L"Floppy Bird", 640, 480);

	window = this;
}

void FloppyBird::Windowing::WindowWin32::Show() {
	if (hwnd) {
		ShowWindow(hwnd, SW_SHOW);
	}
}

bool FloppyBird::Windowing::WindowWin32::ProcessMessages() {
	MSG msg;
	if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return msg.message != WM_QUIT;
}

void FloppyBird::Windowing::WindowWin32::GetScreenSize(uint32_t& w, uint32_t& h) {
	RECT clientRect = {};
	::GetClientRect(hwnd, &clientRect);

	w = clientRect.right - clientRect.left;
	h = clientRect.bottom - clientRect.top;
}

HWND FloppyBird::Windowing::WindowWin32::GetHandle() {
	return hwnd;
}
