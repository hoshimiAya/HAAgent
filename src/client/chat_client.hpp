#pragma once
#include <string>

#include "../http/http_client.hpp"

class ChatClient {
public:
	ChatClient(const std::string& host, const std::string& port);

	// 启动
	void run();

private:
	HttpClient http_client_;
	std::string session_id_;
};