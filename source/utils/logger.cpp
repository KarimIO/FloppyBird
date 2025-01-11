#include <format>
#include <fstream>
#include <iostream>

#include <windowing/windowWin32.hpp>

#ifdef _MSC_VER
#include <Windows.h>
#define USE_WINDOWS_OUTPUT true
#else
#define USE_WINDOWS_OUTPUT false
#endif

#include "logger.hpp"

const char* logLevelStrings[]{
	"Trace",
	"Info",
	"Warning",
	"Error",
	"Fatal"
};

const char* logSourceStrings[]{
	"Main",
	"Windows",
	"DirectX",
	"Renderer",
	"Physics",
	"Input",
	"Audio",
	"Gameplay"
};

#if USE_WINDOWS_OUTPUT
WORD consoleColors[] = {
	FOREGROUND_GREEN, // Info
	FOREGROUND_BLUE, // Trace
	FOREGROUND_RED | FOREGROUND_GREEN, // Warning
	FOREGROUND_RED, // Error
	BACKGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_BLUE // Fatal
};
#else
const char* consoleColors[] = {
	"\033[032m", // Info
	"\033[034m", // Trace
	"\033[033m", // Warning
	"\033[031m", // Error
	"\033[031m" // Fatal
};
#endif

namespace FloppyBird::Logger {
	struct LoggerContext {
		std::ofstream outputStream;
		FloppyBird::Windowing::Window* window = nullptr;
	} context;
}

void FloppyBird::Logger::Initialize(const std::filesystem::path& path) {
	context.outputStream = std::ofstream(path);
}

void FloppyBird::Logger::SetWindow(FloppyBird::Windowing::Window* window) {
	context.window = window;
}

void FloppyBird::Logger::Log(LogLevel level, LogSource source, const char* sourcefile, size_t line, const char* msg) {
	const char* levelStr = logLevelStrings[static_cast<uint8_t>(level)];
	const char* sourceStr = logSourceStrings[static_cast<uint8_t>(source)];
	std::string formattedHeader = std::format("[{}] [{}] {}:{}", levelStr, sourceStr, sourcefile, line, msg);
	
	auto consoleColor = consoleColors[static_cast<uint8_t>(level)];

	std::string joinedStringSimple = std::format("{}: {}\n", formattedHeader, msg);
	if (context.outputStream) {
		context.outputStream << joinedStringSimple;
		context.outputStream.flush();
	}

	#if USE_WINDOWS_OUTPUT

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, consoleColor);
	std::cout << formattedHeader << ": ";
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	std::cout << msg << '\n';

	OutputDebugString(joinedStringSimple.c_str());

	#else
	
	std::string joined = std::format("{}: {}\033[0m{}\n", consoleColor, formattedHeader, msg);
	std::cout << joined;

	#endif

	if (level == LogLevel::Fatal) {
#ifdef _MSC_VER
		MessageBoxA(0, msg, formattedHeader.c_str(), MB_ICONERROR | MB_OK | MB_DEFAULT_DESKTOP_ONLY);
#endif
		DebugBreak();
		exit(1);
	}
}
