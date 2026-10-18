#pragma once
#include <string>

#include "llm_client.hpp"

class ChatEngine {
public:
	ChatEngine();

	// 主入口
	std::string process(const std::string& summary, const std::vector<std::pair<std::string, std::string>>& history, const std::string& input);

	// 把旧摘要和溢出消息变成新摘要
	std::string summarize(const std::string& old_summary, const std::vector<std::pair<std::string, std::string>>& overflow);

private:
	LLMClient llm_client_;
};