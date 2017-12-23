#include "logger.h"

using namespace chainthings;

std::shared_ptr<spdlog::logger> chainthings::g_console_logger = spdlog::stdout_color_mt("console_spdlog");
#if defined (ENABLE_FILE_LOGGING)
std::shared_ptr<spdlog::logger> chainthings::g_file_logger = spdlog::daily_logger_mt("daily_logger", "log.txt");
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