#pragma once

#include <map>
#include <functional>

#include <Windows.h>

namespace FloppyBird::Windowing {
	class Window {
	public:
#ifdef _MSC_VER
		static Window* Create(HINSTANCE hInstance);
#else
		static Window* Create();
#endif

		void SetOnResizeCallback(std::function<void(uint32_t, uint32_t)> onResizeCallback);
		void InvokeOnResize(uint32_t w, uint32_t h);

		void SetOnKeyPressCallback(WPARAM key, std::function<void()> callback);
		void InvokeOnKeypress(WPARAM key);

		// Returns if should quit.
		virtual void Show() = 0;
		virtual void GetScreenSize(uint32_t& w, uint32_t& h) = 0;
		virtual bool ProcessMessages() = 0;

	protected:
		std::map<WPARAM, std::function<void()>> keymaps;
		std::function<void(uint32_t, uint32_t)> onResizeCallback;
	};
}
