#include <utils/logger.hpp>

#include "window.hpp"
#include "windowWin32.hpp"

#ifdef _MSC_VER

FloppyBird::Windowing::Window* FloppyBird::Windowing::Window::Create(HINSTANCE hInstance) {
	return new WindowWin32(hInstance);
}

#else

FloppyBird::Windowing::Window* FloppyBird::Windowing::Window::Create() {
	F_LOG_FATAL(FloppyBird::Logger::LogSource::Windows, "This platform is not supported.");
	return nullptr;
}

#endif

void FloppyBird::Windowing::Window::SetOnResizeCallback(std::function<void(uint32_t, uint32_t)> onResizeCallback) {
	this->onResizeCallback = onResizeCallback;
}

void FloppyBird::Windowing::Window::InvokeOnResize(uint32_t w, uint32_t h) {
	onResizeCallback(w, h);
}

void FloppyBird::Windowing::Window::SetOnKeyPressCallback(WPARAM key, std::function<void()> callback) {
	keymaps[key] = callback;
}

void FloppyBird::Windowing::Window::InvokeOnKeypress(WPARAM key) {
	const auto& keyIt = keymaps.find(key);
	if (keyIt != keymaps.end()) {
		keyIt->second();
	}
}
