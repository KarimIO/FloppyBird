#include <windowing/window.hpp>
#include <graphics/dx_context.hpp>
#include <gameplay/scene.hpp>
#include <rendering/scene_renderer.hpp>
#include <utils/logger.hpp>

#ifdef _MSC_VER
int CALLBACK wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
#else
int main(int argc, char** argv) {
#endif
	FloppyBird::Logger::Initialize(std::filesystem::current_path() / "output.log");

	F_LOG_INFO(FloppyBird::Logger::LogSource::Main, "Floppy Bird loading...");

	FloppyBird::Windowing::Window* window = FloppyBird::Windowing::Window::Create(hInstance);
	if (window == nullptr) {
		return 1;
	}

	FloppyBird::Logger::SetWindow(window);
	FloppyBird::Graphics::DX::Context context;
	if (!context.Initialize(window)) {
		return 1;
	}

	FloppyBird::Rendering::Initialize(context);
	FloppyBird::Rendering::SceneRenderer sceneRenderer;
	FloppyBird::Gameplay::Scene scene;
	scene.Initialize(context);

	window->Show();

	F_LOG_INFO(FloppyBird::Logger::LogSource::Main, "Floppy Bird loaded.");

	std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point currentTime;

	while (true) {
		bool shouldQuit = window->ProcessMessages();
		if (!shouldQuit) {
			break;
		}
		
		currentTime = std::chrono::steady_clock::now();
		double dt = std::chrono::duration<double>(currentTime - lastTime).count();
		lastTime = currentTime;

		scene.Update(dt);
		sceneRenderer.Render(context, scene);
	}

	F_LOG_INFO(FloppyBird::Logger::LogSource::Main, "Floppy Bird closed.");
}
