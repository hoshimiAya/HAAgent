#pragma once
#include <string>
#include <vector>

class LLMClient {
public:
	LLMClient(const std::string& api_key);

	// 检查 API Key 是否配置
	bool is_configured() const;

	// 发送消息并获取模型的回复
	std::string complete(const std::vector<std::pair<std::string, std::string>>& message);

private:
	// 发送消息并获取模型的回复
	std::string llm_response(const std::vector<std::pair<std::string, std::string>>& message);

private:
	std::string api_key_;
};