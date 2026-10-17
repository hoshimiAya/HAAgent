#pragma once
#include <string>
#include <vector>

#include "../http/http_client.hpp"

class LLMClient {
public:
	LLMClient(const std::string& host, const std::string& port);

	// 发送消息并获取模型的回复
	std::string complete(const std::vector<std::pair<std::string, std::string>>& message);

private:
	// 发送消息并获取模型的回复
	std::string llm_response(const std::vector<std::pair<std::string, std::string>>& message);

private:
	HttpClient http_client_;
};