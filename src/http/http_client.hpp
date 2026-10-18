#pragma once
#include <string>
#include <map>

#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

using HttpResponse = http::response<http::string_body>;

class HttpClient {
public:
	HttpClient(const std::string& host, const std::string& port);
	
	// 发送post请求
	std::string post(const std::string& target, const std::map<std::string, std::string>& headers, const std::string& body);

	// 发送post请求，返回完整响应
	HttpResponse post_full(const std::string& target, const std::map<std::string, std::string>& headers, const std::string& body);

private:
	std::string host_;
	std::string port_;
};