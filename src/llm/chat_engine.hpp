#pragma once
#include <string>
#include <vector>

#include "llm_client.hpp"

class ChatEngine {
public:
	ChatEngine();

	// 主入口
	std::string process(const std::string& input);

	// 检查配置
	bool is_configured();

private:
	// 获取API KEY
	std::string get_api_key();

private:
	LLMClient llm_client_;
};