#pragma once
#include <string>
#include <map>

class HttpClient {
public:
	HttpClient(const std::string& host, const std::string& port);
	
	// 发送post请求
	std::string post(const std::string& target, const std::map<std::string, std::string>& headers, const std::string& body);

private:
	std::string host_;
	std::string port_;
};