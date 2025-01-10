#pragma once

#include <stdint.h>
#include <filesystem>
#include <format>

namespace FloppyBird::Windowing {
	class Window;
}

namespace FloppyBird::Logger {
	enum class LogLevel : uint8_t {
		Trace,
		Info,
		Warning,
		Error,
		Fatal
	};

	enum class LogSource : uint8_t {
		Main,
		Windows,
		DirectX,
		Renderer,
		Physics,
		Input,
		Audio,
		Gameplay
	};

	void Initialize(const std::filesystem::path& path);
	void SetWindow(FloppyBird::Windowing::Window* window);
	void Log(LogLevel level, LogSource source, const char* sourcefile, size_t line, const char* msg);

	template<typename... Args>
	void LogV(
		LogLevel level,
		LogSource source,
		const char* sourcefile,
		uint32_t line,
		const char* fmtStr,
		Args... formatItems
	) {
		std::string formattedString = std::vformat(fmtStr, std::make_format_args(std::forward<Args>(formatItems)...));
		FloppyBird::Logger::Log(level, source, sourcefile, line, formattedString.c_str());
	}
}

#define F_LOG(level, source, msg) FloppyBird::Logger::Log(level, source, __FILE__, __LINE__, msg)
#define F_LOG_TRACE(source, msg) F_LOG(FloppyBird::Logger::LogLevel::Trace, source, msg)
#define F_LOG_INFO(source, msg) F_LOG(FloppyBird::Logger::LogLevel::Info, source, msg)
#define F_LOG_WARN(source, msg) F_LOG(FloppyBird::Logger::LogLevel::Warning, source, msg)
#define F_LOG_ERROR(source, msg) F_LOG(FloppyBird::Logger::LogLevel::Error, source, msg)
#define F_LOG_FATAL(source, msg) F_LOG(FloppyBird::Logger::LogLevel::Fatal, source, msg)

#define F_LOG_V(level, source, msg, ...) FloppyBird::Logger::LogV(level, source, __FILE__, __LINE__, msg, __VA_ARGS__)
#define F_LOG_TRACE_V(source, msg, ...) FloppyBird::Logger::LogV(FloppyBird::Logger::LogLevel::Trace, source, __FILE__, __LINE__, msg, __VA_ARGS__)
#define F_LOG_INFO_V(source, msg, ...) FloppyBird::Logger::LogV(FloppyBird::Logger::LogLevel::Info, source, __FILE__, __LINE__, msg, __VA_ARGS__)
#define F_LOG_WARN_V(source, msg, ...) FloppyBird::Logger::LogV(FloppyBird::Logger::LogLevel::Warning, source, __FILE__, __LINE__, msg, __VA_ARGS__)
#define F_LOG_ERROR_V(source, msg, ...) FloppyBird::Logger::LogV(FloppyBird::Logger::LogLevel::Error, source, __FILE__, __LINE__, msg, __VA_ARGS__)
#define F_LOG_FATAL_V(source, msg, ...) FloppyBird::Logger::LogV(FloppyBird::Logger::LogLevel::Fatal, source, __FILE__, __LINE__, msg, __VA_ARGS__)
