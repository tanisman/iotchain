#include "logger.h"

using namespace chainthings::p2p;

std::shared_ptr<spdlog::logger> chainthings::p2p::g_console_logger = spdlog::stdout_color_mt("p2p-network");
#if defined (ENABLE_FILE_LOGGING)
std::shared_ptr<spdlog::logger> chainthings::p2p::g_file_logger = spdlog::daily_logger_mt("network", "network_log.txt");
#endif //ENABLE_FILE_LOGGING

logger::logger(log_level lvl)
	: log_lvl_(lvl)
{
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