#pragma once
#include <string>

#include "llm_client.hpp"

class ChatEngine {
public:
	ChatEngine();

	// 主入口
	std::string process(const std::string& input);

private:
	LLMClient llm_client_;
};