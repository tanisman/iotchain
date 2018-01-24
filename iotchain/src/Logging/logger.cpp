#include "logger.h"

using namespace chainthings;

bool chainthings::g_logger_initialized = false;
std::shared_ptr<spdlog::logger> chainthings::g_console_logger = nullptr;
#if defined (ENABLE_FILE_LOGGING)
std::shared_ptr<spdlog::logger> chainthings::g_file_logger = nullptr;
#endif //ENABLE_FILE_LOGGING

logger::logger(log_level lvl)
	: log_lvl_(lvl)
{
	if (!g_logger_initialized)
	{
		spdlog::set_async_mode(4096, spdlog::async_overflow_policy::block_retry,
			nullptr,
			std::chrono::seconds(2));
		g_console_logger = spdlog::stdout_color_st("p2p-network");
#if defined (ENABLE_FILE_LOGGING)
		g_file_logger = spdlog::daily_logger_st("network", "network_log.txt");
#endif //ENABLE_FILE_LOGGING
		g_logger_initialized = true;
	}
	spdlog::set_level(LOG_LEVEL);
}

logger& logger::operator<<(const std::string& log)
{
	g_console_logger->log(this->log_lvl_, log);
#if defined (ENABLE_FILE_LOGGING)
	g_file_logger->log(this->log_lvl_, log);
#endif //ENABLE_FILE_LOGGING
	return *this;
}