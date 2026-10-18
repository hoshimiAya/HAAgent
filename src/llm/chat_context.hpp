#pragma once
#include <string>
#include <chrono>
#include <vector>

struct ChatContext
{
	std::string session_id;
	std::string last_intent;
	std::string last_topic;
	std::chrono::steady_clock::time_point last_time;
	int message_out = 0;
	std::vector<std::pair<std::string, std::string>> history;
	std::string summary; // 摘要

	bool is_valid() const {
		if (last_intent.empty()) {
			return false;
		}
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_time).count();
		return elapsed < 30;
	}
};