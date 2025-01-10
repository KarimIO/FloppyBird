#pragma once

#ifdef _MSC_VER
#include <Windows.h>
#include <utils/logger.hpp>

namespace FloppyBird::Utils {
	void PrintHResult(HRESULT res, FloppyBird::Logger::LogLevel errorLogLevel);
	void ThrowIfFailed(HRESULT res) ;
}
#endif
