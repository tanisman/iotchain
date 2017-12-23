#pragma once

#include "../Logging/spdlog/spdlog.h"

#define ENABLE_FILE_LOGGING
#if defined (_DEBUG)
#	define LOG_LEVEL	spdlog::level::debug
#else
#	define LOG_LEVEL	spdlog::level::info
#endif //_DEBUG
#define LOG_DEBUG(log)		chainthings::logger(spdlog::level::debug) << log
#define LOG_INFO(log)		chainthings::logger(spdlog::level::info) << log
#define LOG_WARN(log)		chainthings::logger(spdlog::level::warn) << log
#define LOG_ERR(log)		chainthings::logger(spdlog::level::err) << log
#define LOG_CRITICAL(log)	chainthings::logger(spdlog::level::critical) << log

namespace chainthings {

using log_level = spdlog::level::level_enum;

class logger
{
public:
	logger(log_level lvl);	
	logger& operator<<(const std::string& log);
	template<typename... Args>
	logger& operator()(const std::string& log, Args&&... args)
	{
		g_console_logger->log(this->log_lvl_, log, std::forward<Args>(args)...);
#if defined (ENABLE_FILE_LOGGING)
		g_file_logger->log(this->log_lvl_, log, std::forward<Args>(args)...);
#endif //ENABLE_FILE_LOGGING
	}
private:
	log_level log_lvl_;
};

extern std::shared_ptr<spdlog::logger> g_console_logger;
#if defined (ENABLE_FILE_LOGGING)
extern std::shared_ptr<spdlog::logger> g_file_logger;
#endif //ENABLE_FILE_LOGGING
}