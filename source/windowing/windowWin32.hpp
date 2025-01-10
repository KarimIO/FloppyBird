#pragma once

#include "window.hpp"

namespace FloppyBird::Windowing {
	class WindowWin32 : public Window {
	public:
		WindowWin32(HINSTANCE hInstance);

		virtual void Show() override;
		virtual bool ProcessMessages() override;
		virtual void GetScreenSize(uint32_t& w, uint32_t& h) override;

		HWND GetHandle();
	protected:
		HINSTANCE hinstance;
		HWND hwnd;
	};
}
