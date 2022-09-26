#include <spdlog/spdlog.h>

class Logger
{
public:
	template<typename... Args>
	static void log_error(const char* msg, Args &&... args)
	{
		spdlog::error(msg, args);
	}

	template<typename... Args>
	static void log_warning(const char* msg, Args &&... args)
	{
		spdlog::warn(msg, args);
	}

	template<typename... Args>
	static void log_debug(const char* msg, Args &&... args)
	{
		spdlog::debug(msg, args);
	}

	template<typename... Args>
	static void log_info(const char* msg, Args &&... args)
	{
		spdlog::info(msg, args);
	}
};
