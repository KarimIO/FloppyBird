#include "win32_helpers.hpp"

void FloppyBird::Utils::PrintHResult(HRESULT res, FloppyBird::Logger::LogLevel errorLogLevel = FloppyBird::Logger::LogLevel::Error) {
	FloppyBird::Logger::LogLevel logLevel = res == 0
		? FloppyBird::Logger::LogLevel::Info
		: errorLogLevel;
	
	char* errorMsg;
	if (!FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			res,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
			(LPTSTR)&errorMsg,
			0,
			NULL
		)
	) {
		F_LOG_V(logLevel, FloppyBird::Logger::LogSource::DirectX, "HRESULT: unknown error {}", res);
		return;
	}

	F_LOG_V(logLevel, FloppyBird::Logger::LogSource::DirectX, "HRESULT: {}", errorMsg);
	LocalFree(errorMsg);
}

void FloppyBird::Utils::ThrowIfFailed(HRESULT res) {
	if (FAILED(res)) {
		PrintHResult(res, FloppyBird::Logger::LogLevel::Fatal);
#if defined(_DEBUG)
		DebugBreak();
#endif
		exit(1);
	}
}
