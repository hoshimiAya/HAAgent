#pragma once
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace logging {

inline void init(spdlog::level::level_enum level = spdlog::level::info)
{
	if (spdlog::get("server")) {
		return;
	}

	auto logger = spdlog::stdout_color_mt("server");
	logger->set_level(level);
	logger->set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$] %v");
	logger->flush_on(spdlog::level::warn);
	spdlog::set_default_logger(logger);
}

inline std::string short_id(const std::string& id)
{
	return id.size() <= 8 ? id : id.substr(0, 8);
}

} // namespace logging

#define LOG_DBG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INF(...) spdlog::info(__VA_ARGS__)
#define LOG_WRN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERR(...) spdlog::error(__VA_ARGS__)